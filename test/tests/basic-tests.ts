import { XdeltaAlgorithm, BlockMatchSpeed, DeltaGenerationErrorCodes } from '../../xdeltaalgorithm';
import { readFileSync, readdirSync } from 'fs';
import { fail, ok, equal } from 'assert';
import { join } from 'path';

const testDataDir = join(__dirname, '..', 'testData', 'basic-tests');

declare type TestDataPaths = {
	dictionary: string,
	target: string
}

function getTestCases(): string[] {
	return readdirSync(testDataDir);
}

function getTestDataPaths(testCase: string): TestDataPaths {
	const testCaseDir = join(testDataDir, testCase);
	return {
		dictionary: join(testCaseDir, 'dictionary'),
		target: join(testCaseDir, 'target')
	};
}

describe('XdeltaAlgorithm', () => {
	const xdelta = new XdeltaAlgorithm();
	const maxResultSize = 100000000;
	const testCases = getTestCases();
	[
		BlockMatchSpeed.Default,
		BlockMatchSpeed.Slow,
		BlockMatchSpeed.Fast,
		BlockMatchSpeed.Faster,
		BlockMatchSpeed.Fastest
	].forEach(speed => {
		testCases.forEach(testCase => {
			it(`should properly generate and apply patches (BlockMatchSpeed.${BlockMatchSpeed[speed]}) - ${testCase}`, done => {
				const testDataPaths = getTestDataPaths(testCase);
				const dictionary = readFileSync(testDataPaths.dictionary);
				const target = readFileSync(testDataPaths.target);
				xdelta.generateDelta(dictionary, target, maxResultSize, speed, result => {
					if(result.error) {
						fail(`Delta generation failed. Code: ${result.error.code}, message: ${result.error.message}`);
						done();
						return;
					}
					if(result.result === undefined) {
						fail('Successful delta generation should produce result.');
						done();
						return;
					}
					xdelta.applyDelta(dictionary, result.result, target.length, result => {
						if(result.error) {
							fail(`Delta application failed. Code: ${result.error.code}, message: ${result.error.message}`);
							done();
							return;
						}
						if(result.result === undefined) {
							fail('Successful delta application should produce result.');
							done();
							return;
						}
						ok(target.equals(result.result), 'Delta application result does not match the expected result.');
						done();
					});
				});
			});
		});
	});

	it('should properly cancel delta generation', done => {
		const testDataPaths = getTestDataPaths(testCases[0]);
		const dictionary = readFileSync(testDataPaths.dictionary);
		const target = readFileSync(testDataPaths.target);
		const encoderId = xdelta.generateDelta(dictionary, target, maxResultSize, BlockMatchSpeed.Fastest, result => {
			if(!result.error) {
				fail('Cancelled delta generation should have error set.');
				done();
				return;
			}
			if(result.result) {
				fail('Cancelled delta generation should not have result set.');
				done();
				return;
			}
			equal(result.error.code, DeltaGenerationErrorCodes.XD3_CANCELLED, 'Cancelled delta generation should have XD3_CANCELLED error set.');
			done();
		});
		if(encoderId === undefined) {
			fail('generateDelta() should return encoder id.');
			done();
			return;
		}
		const error = xdelta.cancelGeneration(encoderId);
		if(error) {
			fail(`Delta generation cancellation failed. Code: ${error.code}, message: ${error.message}`);
			done();
		}
	});
});
