module BindingHelpers {
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

	export type BindWrapperTargetResult<
		TargetType extends Object,
		Key extends string,
		Wrapper extends DataWrapper<any>
	> = TargetType & {
		[K in Key]: Wrapper extends DataWrapper<infer T> ? T : never;
	};

	/**
	 *
	 * @param key Key on destination to bind the wrapper to.
	 * @param wrapper Data wrapper provided by the API to bind to the destination.
	 * @param destination Destination to bind the wrapper on. Defaults to the module exports.
	 */
	export function bindWrapper<
		T,
		K extends string,
		TargetType extends Object,
		Wrapper extends DataWrapper<T>
	>(
		key: K,
		wrapper: DataWrapper<T>,
		destination: TargetType = module.exports
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
			callback: (
				value: T,
				index: number,
				array: ArrayAccessor<T>
			) => boolean
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
			callback: (
				value: T,
				index: number,
				array: ArrayAccessor<T>
			) => boolean
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
	type SetOnlyMembers<Members extends Record<string, DataWrapper<any>>> =
		Exclude<
			KeysMatching<Members, DataWrapperSetOnly<any>>,
			KeysMatching<Members, DataWrapperReadOnly<any>>
		>;
	type SetableMembers<Members extends Record<string, DataWrapper<any>>> =
		KeysMatching<Members, DataWrapperSetOnly<any>>;

	export type ClassPrototype<
		Members extends Record<string, DataWrapper<any>>
	> = {
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
		MemberFields extends Record<string, any>,
		StaticFields extends Record<string, any>,
		StaticFieldNames extends string = StaticFields extends Record<
			infer N,
			any
		>
			? N
			: never
	> = {
		new (...params: ConstructorParameters): ClassPrototype<MemberFields>;
	} & {
		[K in StaticFieldNames]: BindWrapperTargetResult<
			{},
			K,
			StaticFields[K]
		>[K];
	};

	class InitialEmptyConstructor {}

	class BindAsSrc<
		MemberFields extends Record<string, DataWrapperOptions<any>>
	> {
		constructor(public source: MemberFields) {}
	}

	export class APIClass {
		constructor() {}
	}

	/**
	 *
	 * @param className The name of the generated class
	 * @param constructor The function which generates the DataWrapper to bind as the class instance
	 * @param staticFields Static fields which are bound to the actual class object.
	 * @returns An object containing the generatedClass and a bindAs function which can be used by the
	 * 	C++ calling function to bind an object as an instance of the class.
	 */
	export function makeClass<
		ConstructorParameters extends unknown[],
		MemberFields extends Record<string, DataWrapperOptions<any>>,
		StaticFields extends Record<string, DataWrapperOptions<any>>,
		ClassName extends string
	>(
		className: ClassName,
		constructor: ClassConstructor<ConstructorParameters, MemberFields>,
		staticFields: StaticFields
	): {
		generatedClass: Class<
			ConstructorParameters,
			MemberFields,
			StaticFields
		>;
		bindAs: (sourceObject: MemberFields) => ClassPrototype<MemberFields>;
	} {
		/*
		Trick to get the class name to match what was passed into the function.
		This way, `class.name` will return that value.
	*/
		const classContainer = {
			[className]: class extends APIClass {
				constructor(
					...params:
						| ConstructorParameters
						| [InitialEmptyConstructor]
						| [BindAsSrc<MemberFields>]
				) {
					super();
					/*
					Trick to create an object that is an instance of the class,
					that way we can bind all of the member fields dynamically
					onto the object. Since the object was created with an actual
					constructor, it is an actual instance of the class, so the
					`instanceof` operator will still work.

					This is a workaround since TSTL does not support `Object.setPrototypeOf`.
				*/
					if (params[0] instanceof InitialEmptyConstructor) {
						return;
					}
					let constructorResult: MemberFields;
					if (params[0] instanceof BindAsSrc) {
						constructorResult = params[0].source;
					} else {
						constructorResult = constructor(
							...(params as ConstructorParameters)
						);
					}

					const constructed = new classContainer[className](
						new InitialEmptyConstructor()
					);

					for (const [key, value] of Object.entries(
						constructorResult
					)) {
						bindWrapper(key, value, constructed);
					}

					return constructed as ClassPrototype<MemberFields>;
				}
			}
		};

		for (const [key, value] of Object.entries(staticFields)) {
			bindWrapper(key, value, classContainer[className] as any);
		}

		const generatedClass = classContainer[className] as Class<
			ConstructorParameters,
			MemberFields,
			StaticFields
		>;

		const generatedBindAsClass: {
			new (source: BindAsSrc<MemberFields>): ClassPrototype<MemberFields>;
		} = generatedClass as unknown as any;

		return {
			generatedClass,
			bindAs: (sourceObject: MemberFields) =>
				new generatedBindAsClass(
					new BindAsSrc<MemberFields>(sourceObject)
				)
		};
	}
}

const moduleWrapper = { BindingHelpers };
type ModuleName = keyof typeof moduleWrapper;
export const bindingHelpersModuleName: ModuleName = Object.keys(
	moduleWrapper
)[0] as ModuleName;
export const moduleResolutionPath = 'binding-helpers';

export default BindingHelpers;
