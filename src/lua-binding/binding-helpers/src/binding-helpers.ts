/**
 * A wrapper object passed into the runtime to bind APIs.
 */
export type DataWrapper<T> = {
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
};

export type DataWrapperReadOnly<T> = Required<Pick<DataWrapper<T>, 'get'>>;
export type DataWrapperSetOnly<T> = Required<Pick<DataWrapper<T>, 'set'>>;
export type DataWrapperReadSet<T> = Required<DataWrapper<T>>;
export type DataWrapperOptions<T> =
	| DataWrapperReadOnly<T>
	| DataWrapperSetOnly<T>
	| DataWrapperReadSet<T>;

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
	Object.defineProperty(destination, key, wrapper);
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

type KeysMatching<T, V> = {
	[K in keyof T]-?: T[K] extends V ? K : never;
}[keyof T];

type ReadOnlyMembers<Members extends Record<string, DataWrapper<any>>> =
	Exclude<
		KeysMatching<Members, DataWrapperReadOnly<any>>,
		KeysMatching<Members, DataWrapperSetOnly<any>>
	>;
type SetOnlyMembers<Members extends Record<string, DataWrapper<any>>> = Exclude<
	KeysMatching<Members, DataWrapperSetOnly<any>>,
	KeysMatching<Members, DataWrapperReadOnly<any>>
>;
type SetableMembers<Members extends Record<string, DataWrapper<any>>> =
	KeysMatching<Members, DataWrapperSetOnly<any>>;

export type ClassPrototype<Members extends Record<string, DataWrapper<any>>> = {
	readonly [K in ReadOnlyMembers<Members>]: Members[K] extends DataWrapperReadOnly<
		infer T
	>
		? T
		: never;
} & {
	[K in SetableMembers<Members>]: Members[K] extends DataWrapperSetOnly<
		infer T
	>
		? T
		: never;
};

export type ClassConstructor<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, DataWrapper<any>>
> = (...params: ConstructorParameters) => MemberFields;

export type Class<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, any>
> = {
	new (...params: ConstructorParameters): ClassPrototype<MemberFields>;
};

export function makeClass<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, DataWrapperOptions<any>>
>(
	className: string,
	constructor: ClassConstructor<ConstructorParameters, MemberFields>
): Class<ConstructorParameters, MemberFields> {
	const createdClass = class {
		constructor(...params: ConstructorParameters) {
			const constructorResult = constructor(...params);
			const constructed = {};

			for (const [key, value] of Object.entries(constructorResult)) {
				bindWrapper(key, value, constructed);
			}

			Object.setPrototypeOf(constructed, createdClass.prototype);
			return constructed as ClassPrototype<MemberFields>;
		}
	};

	return createdClass as {
		new (...params: ConstructorParameters): ClassPrototype<MemberFields>;
	};
}

declare class B {}

let C = makeClass('C', (a: number, b: string) => ({
	value: {
		get: () => {
			return `${a}: ${b}`;
		},
		set: (s: string) => {}
	}
}));
let D = makeClass('C', (a: number, b: string) => ({
	value: {
		get: () => {
			return `${a}: ${b}`;
		},
		set: (s: string) => {}
	}
}));

let c1 = new C(123, 'abc');

console.log(c1);
console.log(c1 instanceof C);
console.log(c1 instanceof D);
console.log(c1.value);
c1.value = '';

let d1 = new D(123, 'abc');
console.log(d1);
console.log(d1 instanceof D);
console.log(d1 instanceof C);
console.log(d1.value);

console.log(c1 === d1);
