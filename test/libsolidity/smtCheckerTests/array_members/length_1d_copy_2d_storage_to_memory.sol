pragma experimental SMTChecker;
pragma experimental ABIEncoderV2;

contract C {
	uint[][] arr;
	function f() public {
		uint[][] memory arr2 = arr;
		assert(arr2[0].length == arr[0].length);
	}
}
