import { readFileSync, writeFileSync } from 'fs';

import { XdeltaAlgorithm, BlockMatchSpeed, DeltaGenerationCallbackArg } from 'xdelta';

const files = [];
for (let i = 2; i < process.argv.length; i++) {
	files[i - 2] = process.argv[i];
}

const xdelta = new XdeltaAlgorithm();

function encode(id: number, dictionaryFile: string, targetFile: string, deltaFile: string, decodedFile: string) {
	console.log(`${id} - Generating delta...`);

	const dictionary = readFileSync(dictionaryFile);
	const target = readFileSync(targetFile);

	const maxResultSize = 100000000;

	console.time(`${id} - Time needed to generate the delta`);
	const encoderId = xdelta.generateDelta(dictionary,
						target,
						maxResultSize,
						BlockMatchSpeed.Fastest,
						(result: DeltaGenerationCallbackArg) => {
							if (result.error) {
								console.log(`${id} - Error occurred during delta generation. Code: ${result.error.code}, message: ${result.error.message}`);
								return;
							}

							console.timeEnd(`${id} - Time needed to generate the delta`);

							writeFileSync(deltaFile, result.result);
							console.log(`${id} - Delta generated successfully.`);

							decode(id, dictionaryFile, deltaFile, decodedFile);
						});
	if (encoderId && encoderId % 2) {
		const err = xdelta.cancelGeneration(encoderId);
		if (err) {
			console.log(err);
		}
	}
}

function decode(id: number, dictionaryFile: string, deltaFile: string, decodedFile: string) {
	console.log(`${id} - Applying delta...`);

	const dictionary = readFileSync(dictionaryFile);
	const delta = readFileSync(deltaFile);

	const maxResultSize = 400000000;

	console.time(`${id} - Time needed to apply the delta`);
	xdelta.applyDelta(dictionary,
						delta,
						maxResultSize,
						(result: DeltaGenerationCallbackArg) => {
							if (result.error) {
								console.log(`${id} - Error occurred during delta application. Code: ${result.error.code}, message: ${result.error.message}`);
								return;
							}

							console.timeEnd(`${id} - Time needed to apply the delta`);

							writeFileSync(decodedFile, result.result);
							console.log(`${id} - Delta applied successfully.`);
						});
}

if (files.length === 4) {
	for (let i = 0; i < 1; i++) {
		encode(i, files[0], files[1], files[2], files[3]);
	}
} else {
	console.log('Usage: npm start -- (oldfile) (newfile) (deltafile) (resultfile)');
}