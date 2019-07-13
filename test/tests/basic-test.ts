import { XdeltaAlgorithm, BlockMatchSpeed, DeltaGenerationCallbackArg  } from '../../xdeltaalgorithm';
import { readFileSync } from 'fs';

const assert = require('assert');
var path = require('path');


const xdelta = new XdeltaAlgorithm();

function encodeAsync(dictionaryFile: string, targetFile: string, completed: (result: DeltaGenerationCallbackArg) => void) {

	const dictionary = readFileSync(dictionaryFile);
	const target = readFileSync(targetFile);

	const maxResultSize = 100000000;

	return xdelta.generateDelta(dictionary,
						target,
						maxResultSize,
						BlockMatchSpeed.Fastest,
						(result: DeltaGenerationCallbackArg) => {
              completed(result);
						});
}

describe('Simple Delta Generation', function() {
    it('Generate 2MB JSON delta', function(done) {
      console.log(__dirname);
      encodeAsync(path.join(__dirname,'..', 'testData', '2MB-old.json'),path.join(__dirname,'..', 'testData','2MB-new.json'), (result: DeltaGenerationCallbackArg) => {
        if(result.error) {
          assert.fail('Delta generation failed.Code: ${result.error.code}, message: ${result.error.message}');
          done();
          return;
        }
      });
      done();
    });
});