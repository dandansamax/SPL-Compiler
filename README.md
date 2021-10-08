# CS323-Project1

## Current achievement

Add basic lexical tokens and syntax analysis.

## Todo

### Basic requirements

- [x] syntax tree Constructing and results print

### Error check

- [x] Error check in lexical analysis (official)
- [x] Error check in syntax analysis (official)
- [ ] Error check in syntax analysis (self)
- [ ] Error check in syntax analysis (self)

Instructions in reference folder will help

#### Error Recovery

- [ ] 小括号左右缺失 （if, while, exp 等）
- [ ] 中括号左右缺失 （数组）
- [ ] 大括号左右缺失 （代码段，结构体声明）
- [ ] 多余分号 `int a;;;`
- [ ] 连续的TYPE符号 `int int a=1;`
- [ ] 声明无内容 `int;`


### Bonus

- [x]  single- and multi-line comment
- [ ]  macro preprocessor
- [ ]  file inclusion
- [ ]  for statements

Bonus part only contain 20 points, So finish Basic part first!

## How to develop

It is a good habit to pull branchs every time you try to modify the codes.

To be convinient, you can switch to a new branch *feature* in local git when you want to add new features. 
After completing, you can merge the branch to main and push it to github. \( doesn't matter if you don't want.\)

We should make sure that the remote main branch always runs correctly.