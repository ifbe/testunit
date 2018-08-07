import math
def func():
	print("func")
	print(math.sqrt(2))
def help():
	print("help")
	func()
if __name__=="__main__":
	print("main")
	help()
