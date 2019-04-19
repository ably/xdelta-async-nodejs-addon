const fs = require('fs');

const xdelta = require('xdelta');

const files = [];
for (let i = 2; i < process.argv.length; i++) {
	files[i - 2] = process.argv[i];
}

const err = xdelta.SetMaxSimultaneouslyRunningEncoders(10);
if (err) {
	console.log(err);
	return;
}

function encode(id, dictionaryFile, targetFile, deltaFile, decodedFile) {
	console.log(`${id} - Generating delta...`);

	const dictionary = fs.readFileSync(dictionaryFile);
	const target = fs.readFileSync(targetFile);

	const resultSize = 100000000;
	const result = Buffer.alloc(resultSize);

	console.time(`${id} - Time spent in the native call on the main thread`);
	console.time(`${id} - Time needed to generate the delta`);
	const encoderId = xdelta.EncodeBuffer(dictionary,
						target,
						result,
						(err, bytesWritten) => {
							if (err) {
								console.log(`${id} - Error occurred during delta generation. Code: ${err.code}, message: ${err.message}`);
								return;
							}

							console.timeEnd(`${id} - Time needed to generate the delta`);
							
							fs.writeFileSync(deltaFile, result.slice(0, bytesWritten));
							console.log(`${id} - Delta generated successfully.`);

							decode(id, dictionaryFile, deltaFile, decodedFile);
						});
	console.timeEnd(`${id} - Time spent in the native call on the main thread`);
	if (encoderId % 2) {
		const err = xdelta.CancelEncode(encoderId);
		if (err) {
			console.log(err);
		}
	}
}

function decode(id, dictionaryFile, deltaFile, decodedFile) {
	console.log(`${id} - Applying delta...`);

	const dictionary = fs.readFileSync(dictionaryFile);
	const delta = fs.readFileSync(deltaFile);

	const resultSize = 400000000;
	const result = Buffer.alloc(resultSize);

	console.time(`${id} - Time spent in the native call on the main thread`);
	console.time(`${id} - Time needed to apply the delta`);
	xdelta.DecodeBuffer(dictionary,
						delta,
						result,
						(err, bytesWritten) => {
							if (err) {
								console.log(`${id} - Error occurred during delta application. Code: ${err.code}, message: ${err.message}`);
								return;
							}

							console.timeEnd(`${id} - Time needed to apply the delta`);

							fs.writeFileSync(decodedFile, result.slice(0, bytesWritten));
							console.log(`${id} - Delta applied successfully.`);
						});
	console.timeEnd(`${id} - Time spent in the native call on the main thread`);
}

if (files.length === 4) {
	for (let i = 0; i < 10; i++) {
		encode(i, files[0], files[1], files[2], files[3]);
	}
} else {
	console.log('Usage: npm start -- (oldfile) (newfile) (deltafile) (resultfile)');
}