pragma experimental SMTChecker;
pragma experimental ABIEncoderV2;

contract C {
	uint[][] arr;
	uint[][] arr2;
	function f() public {
		assert(arr2[0].length == arr[0].length);
	}
}
