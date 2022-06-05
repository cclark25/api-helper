/**
 * A wrapper object passed into the runtime to bind APIs.
 */
export interface DataWrapper<T> {
	/**
	 * Getter function passed in from the API.
	 * @returns A value provided by the API.
	 */
	get?: () => T;
	/**
	 * Setter function passed in from the API.
	 * @param value A value to set in the API.
	 */
	set?: (value: T) => void;
}

/**
 *
 * @param key Key on destination to bind the wrapper to.
 * @param wrapper Data wrapper provided by the API to bind to the destination.
 * @param destination Destination to bind the wrapper on. Defaults to the module exports.
 */
export function bindWrapper<T, K extends string | number>(
	key: K,
	wrapper: DataWrapper<T>,
	destination: Record<K, unknown> = module.exports
) {
	Object.defineProperty(destination, key, {
		...(wrapper.get
			? {
					get: wrapper.get
			  }
			: undefined),
		...(wrapper.set
			? {
					set: wrapper.set
			  }
			: undefined)
	});
}

export interface ExternalArrayAccessor<T> {
	getIndex: (index: number) => T;
	setIndex: (index: number, value: T) => void;
	push: (value: T) => number;
	pop: () => T;
	insert: (index: number, value: T) => number;
	length: () => number;
}

export class ArrayAccessor<T> {
	public static *getArrayIterator<T>(array: ArrayAccessor<T>) {
		for (let i = 0; i < array.length; i++) {
			yield array.getIndex(i);
		}
	}

	public [Symbol.iterator] = () => ArrayAccessor.getArrayIterator(this);

	public constructor(public accessor: ExternalArrayAccessor<T>) {
		this.getIndex = this.accessor.getIndex;
		this.setIndex = this.accessor.setIndex;
		this.push = this.accessor.push;
		this.pop = this.accessor.pop;
		this.insert = this.accessor.insert;
	}

	public getIndex: (index: number) => T;
	public setIndex: (index: number, value: T) => void;
	public push: (value: T) => number;
	public pop: () => T;
	public insert: (index: number, value: T) => number;

	public get length() {
		return this.accessor.length();
	}

	public indexOf(
		callback: (value: T, index: number, array: ArrayAccessor<T>) => boolean
	) {
		let i = 0;
		for (const value of this) {
			if (callback(value, i, this)) {
				break;
			}
			i++;
		}

		return i;
	}

	public find(
		callback: (value: T, index: number, array: ArrayAccessor<T>) => boolean
	) {
		return this.accessor.getIndex(this.indexOf(callback));
	}
}

export function bindArrayWrapper<T>(source: ExternalArrayAccessor<T>) {
	return new ArrayAccessor<T>(source);
}
