import type BindingHelpers from 'binding-helpers';
module TestModule {
	declare class TestClass extends (BindingHelpers.APIClass as {
		new (): { abc: number; def: string };
	}) {
		constructor(p1: number, p2: string);
	}

	export declare class APIState extends (BindingHelpers.APIClass as {
		new (): {
			f1: string;
			f2: number;
			f3: { abc: number; def: string };
			f4: BindingHelpers.ArrayAccessor<{
				a: BindingHelpers.ArrayAccessor<unknown>;
				b: BindingHelpers.ArrayAccessor<string>;
			}>;
			f5: () => unknown;
			f6: (
				param1: string,
				param2: { f1: string; f2: number }
			) => { abc: number };
			TestClass: typeof TestClass;
		};
	}) {
		constructor();
	}
}
export default TestModule;
