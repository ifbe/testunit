fn addsub(x: isize, y:isize) -> (isize, isize) {
    (x + y, x - y)
}
fn main() {
	let result = addsub(5,3);
	println!("add={},sub={}", result.0, result.1);
}
