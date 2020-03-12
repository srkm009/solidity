pragma experimental SMTChecker;

contract C {
	mapping (uint => uint[]) map;
	function f() public {
		assert(map[0].length == map[1].length);
	}
}
