const xdelta = require('bindings')('Xdelta3Addon.node');

export class XdeltaAlgorithm {
	cancelGeneration(encoderId: number): { code: number; message: string; } | undefined {
		return xdelta.CancelEncode(encoderId);
	}

	generateDelta(oldData: Buffer, newData: Buffer, maxDeltaSize: number, speed: BlockMatchSpeed, callback: DeltaGenerationCallback): number | undefined {
		const delta = Buffer.alloc(maxDeltaSize);
		const result = xdelta.EncodeBuffer(oldData, newData, delta, speed, (error: {code: number, message: string}, bytesWritten: number) => {
			if (error) {
				callback({result: undefined, error});
				return;
			}

			// slice does not involve copying
			callback({result: delta.slice(0, bytesWritten)});
		});
		return result;
	}

	generateDeltaAsync(oldData: Buffer, newData: Buffer, maxDeltaSize: number, speed: BlockMatchSpeed): Promise<Buffer> {
		return new Promise((resolve, reject) => {
			this.generateDelta(oldData, newData, maxDeltaSize, speed, result => {
				if (result.error) {
					reject(result.error);
				} else {
					resolve(result.result);
				}
			});
		});
	}

	applyDelta(oldData: Buffer, delta: Buffer, maxResultSize: number, callback: DeltaGenerationCallback): void {
		const result = Buffer.alloc(maxResultSize);
		xdelta.DecodeBuffer(oldData, delta, result, (error: {code: number, message: string}, bytesWritten: number) => {
			if (error) {
				callback({result: undefined, error});
				return;
			}
			// slice does not involve copying
			callback({result: result.slice(0, bytesWritten)});
		});
	}
}

export type DeltaGenerationCallbackArg = {
	result: Buffer;
	error?: undefined;
} | {
	result?: undefined;
	error: {code: number, message: string};
};

export type DeltaGenerationCallback = (deltaOperationCallbackArg: DeltaGenerationCallbackArg) => void;

export enum BlockMatchSpeed {
	Default,
	Slow,
	Fast,
	Faster,
	Fastest,
	Custom
}

export enum DeltaGenerationErrorCodes {
	ENOSPC =    28,  /* No space left on device */
	XD3_CANCELLED = -17715
}
