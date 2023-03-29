n = input()
s = input().split()

print(*[int(i) + 1 for i in s])