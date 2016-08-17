var fs = require('fs');
var proc = require('child_process');
var crypto = require('crypto');

var g_AllTests;
var g_Test0;
var g_Test1;
var g_Template;

/*
Test
	Name: SSE Dot Product
	Domain: xo-math
	Sub-Domain: Dot Product
	Technology: SSE
	Include: {
	}
	Setup: {
		__m128 a = M128RAND;
		__m128 b = M128RAND;
		__m128 mask = _mm_castsi128_ps(_mm_set_epi32(0, 0xffffffff, 0xffffffff, 0xffffffff));
		float dot;
	}
	Run: {
		__m128 temp = _mm_and_ps(_mm_mul_ps(a, b), mask);
		temp = _mm_hadd_ps(tmep, temp);
		temp = _mm_hadd_ps(tmep, temp);
		dot = _mm_cvtss_f32(temp);
	}
*/

function ParseTest(text) {
	var test = {name:"", domain:"", subdomain:"", technology:"", include:"", setup:"", run:""};
	var handleInclude = false;
	var handleSetup = false;
	var handleRun = false;
	for(var i = 0; i < text.length; ++i) {
		if(handleInclude) {
			if(text[i].match(/^}/)) {
				handleInclude = false;
			}
			else {
				test.include += text[i] + '\n';
			}
		}
		else if(handleRun) {
			if(text[i].match(/^}/)) {
				handleRun = false;
			}
			else {
				test.run += '        ' + text[i] + '\n';
			}

		}
		else if(handleSetup) {
			if(text[i].match(/^}/)) {
				handleSetup = false;
			}
			else {
				test.setup += '    ' + text[i] + '\n';
			}

		} 
		else if(text[i].match(/^Name/)) {
			var nameSplit = text[i].split(':');
			test.name = nameSplit[1].trim();
		}
		else if(text[i].match(/^Domain/)) {
			var domainSplit = text[i].split(':');
			test.domain = domainSplit[1].trim();
		}
		else if(text[i].match(/^Sub-Domain/)) {
			var subDomainSplit = text[i].split(':');
			test.subdomain = subDomainSplit[1].trim();
		}
		else if(text[i].match(/^Technology/)) {
			var techSplit = text[i].split(':');
			test.technology = techSplit[1].trim();
		}
		else if(text[i].match(/^Include/)) {
			handleInclude = true;
		}
		else if(text[i].match(/^Setup/)) {
			handleSetup = true;
		}
		else if(text[i].match(/^Run/)) {
			handleRun = true;
		}
	}

	// md5, good enough for what we're doing.
	test.hash = crypto.createHash('md5').update(JSON.stringify(test)).digest('hex');
	return test;
}


function FileLoaded(text) {
	console.log('file loaded. length: ', text.length);
	var lines = text.split('\n');
	console.log('lines of text: ', lines.length);
	var testLines = [];
	var testCount = 0;
	for(var i = 0; i < lines.length; ++i) {
		lines[i] = lines[i].trim();
		if(lines[i].match(/^Test/)) {
			testLines.push([]);
			testCount++;
		}
		else if(testCount > 0 && lines[i].length > 0) {
			testLines[testCount-1].push(lines[i]);
		}
	}

	g_AllTests = [];
	for(var i = 0; i < testLines.length; ++i) {
		g_AllTests.push(ParseTest(testLines[i]));
	}
	RunRandomTest();

}

function TemplateLoaded(text) {
	console.log('template loaded. length: ', text.length);
	g_Template = text;
	RunRandomTest();
}

function TryBeginTest() {
	if(!g_Test0 || !g_Test1 || !g_Template)
		return;
	var lines = g_Template.split('\n');
	var result = "";
	for(var i = 0; i < lines.length; ++i) {
		if(lines[i].indexOf('%includes%') > 0) {
			if(g_Test0.include.length > 0) {
				result += '// Test 0 Includes:\n';
				result += g_Test0.include + '\n';
			}
			if(g_Test1.include.length > 0) {
				result += '// Test 1 Includes:\n';
				result += g_Test1.include + '\n';
			}
		}
		else if(lines[i].indexOf('%test0.name%') > 0) {
			result += '#define NAME_A \"' + g_Test0.name + '\" // generated\n';
		}
		else if(lines[i].indexOf('%test0.hash%') > 0) {
			result += '#define HASH_A \"' + g_Test0.hash + '\" // generated\n';
		}
		else if(lines[i].indexOf('%test0.setup%') > 0) {
			result += '    // Test 0: ' + g_Test0.name + '\n';
			result += '    // Domain: ' + g_Test0.domain + '\n';
			result += '    // Sub-Domain: ' + g_Test0.subdomain + '\n';
			result += '    // Technology: ' + g_Test0.technology + '\n\n';
			result += '    ////////// Setup (generated):\n';
			result += g_Test0.setup;
			result += '    ////////// End Setup (generated):\n';
		}
		else if(lines[i].indexOf('%test0.run%') > 0) {
			result += '        ////////// Run (generated):\n';
			result += g_Test0.run;
			result += '        ////////// End Run (generated):\n';
		}
		else if(lines[i].indexOf('%test1.name%') > 0) {
			result += '#define NAME_B \"' + g_Test1.name + '\" // generated\n';
		}
		else if(lines[i].indexOf('%test1.hash%') > 0) {
			result += '#define HASH_B \"' + g_Test1.hash + '\" // generated\n';
		}
		else if(lines[i].indexOf('%test1.setup%') > 0) {
			result += '    // Test 1: ' + g_Test1.name + '\n';
			result += '    // Domain: ' + g_Test1.domain + '\n';
			result += '    // Sub-Domain: ' + g_Test1.subdomain + '\n';
			result += '    // Technology: ' + g_Test1.technology + '\n\n';
			result += '    ////////// Setup (generated):\n';
			result += g_Test1.setup;
			result += '    ////////// End Setup (generated):\n';
		}
		else if(lines[i].indexOf('%test1.run%') > 0) {
			result += '        ////////// Run (generated):\n';
			result += g_Test1.run;
			result += '        ////////// End Run (generated):\n';
		}
		else if(lines[i].indexOf('%results%') > 0) {
			var filename = '';
			if(g_Test0.hash.localeCompare(g_Test1.hash) < 0) {
				filename = 'Results/' + g_Test0.hash + '_' + g_Test1.hash + '.txt';
			} else {
				filename = 'Results/' + g_Test1.hash + '_' + g_Test0.hash + '.txt';
			}
			result += '#define RESULT_FILE \"' + filename + '\" // generated\n';
		}
		else {
			result += lines[i] + '\n';
		}
	}

	fs.writeFile(__dirname + "/Output/test.cpp", result, function(err) {
		if(err) {
			return console.log(err);
		}

		console.log("The file was saved!");
		CompileOutput();
	}); 
}

function CompileOutput() {
	console.log('compiling...');
	var clang = proc.spawn('clang++', ['-target', 'x86_64-pc-windows-gnu', '-msse4.2', '-O0', '-std=c++11', 'Output/test.cpp']);
	clang.stdout.setEncoding('utf8');
	clang.stdout.on('data', function (data) {
		process.stdout.write(data.toString());
	});

	clang.on('close', function (code) {
		console.log('process exit code ' + code);
		if(code === 0) {
			RunCompiledOutput();
		}
	});
}

function RunCompiledOutput() {
	console.log('running...');
	var application = proc.exec('a.exe');
	application.stdout.setEncoding('utf8');
	application.stdout.on('data', function (data) {
		process.stdout.write(data.toString());
	});

	application.on('close', function (code) {
		console.log('process exit code ' + code);
		if(code === 0) {
			RunRandomTest();
		}
	});
}

function RunRandomTest() {
	g_Test0 = null;
	g_Test1 = null;
	if(g_AllTests && g_AllTests.length > 1) {
		g_Test0 = g_AllTests[Math.floor(Math.random()*g_AllTests.length)];
		g_Test1 = g_AllTests[Math.floor(Math.random()*g_AllTests.length)];
		while(g_Test0 === g_Test1) {
			g_Test1 = g_AllTests[Math.floor(Math.random()*g_AllTests.length)];
		}
	}
	TryBeginTest();
}

fs.readFile( __dirname + '/Test.txt', function (err, data) {
  if (err) {
    throw err; 
  }
  FileLoaded(data.toString());
});

fs.readFile( __dirname + '/Templates/test.cpp', function (err, data) {
  if (err) {
    throw err; 
  }
  TemplateLoaded(data.toString());
});