const fs = require('fs');

const xdelta = require('xdelta');

const files = [];
for (let i = 2; i < process.argv.length; i++) {
	files[i - 2] = process.argv[i];
}

function encode(dictionaryFile, targetFile, deltaFile, decodedFile) {
	console.log('Generating delta...');

	const dictionary = fs.readFileSync(dictionaryFile);
	const target = fs.readFileSync(targetFile);

	const resultSize = 100000000;
	const result = Buffer.alloc(resultSize);

	console.time('Time spent in the native call on the main thread');
	console.time('Time needed to generate the delta');
	xdelta.EncodeBuffer(dictionary,
						target,
						result,
						(err, bytesWritten) => {
							if (err) {
								console.log(`Error occurred during delta generation. Code: ${err.code}, message: ${err.message}`);
								return;
							}

							console.timeEnd('Time needed to generate the delta');
							
							fs.writeFileSync(deltaFile, result.slice(0, bytesWritten));
							console.log("Delta generated successfully.");

							decode(dictionaryFile, deltaFile, decodedFile);
						});
	console.timeEnd('Time spent in the native call on the main thread');
}

function decode(dictionaryFile, deltaFile, decodedFile) {
	console.log('Applying delta...');

	const dictionary = fs.readFileSync(dictionaryFile);
	const delta = fs.readFileSync(deltaFile);

	const resultSize = 400000000;
	const result = Buffer.alloc(resultSize);

	console.time('Time spent in the native call on the main thread');
	console.time('Time needed to apply the delta');
	xdelta.DecodeBuffer(dictionary,
						delta,
						result,
						(err, bytesWritten) => {
							if (err) {
								console.log(`Error occurred during delta application. Code: ${err.code}, message: ${err.message}`);
								return;
							}

							console.timeEnd('Time needed to apply the delta');

							fs.writeFileSync(decodedFile, result.slice(0, bytesWritten));
							console.log('Delta applied successfully.');
						});
	console.timeEnd('Time spent in the native call on the main thread');
}

if (files.length === 4) {
	encode(files[0], files[1], files[2], files[3]);
} else {
	console.log('Usage: npm start -- (oldfile) (newfile) (deltafile) (resultfile)');
}