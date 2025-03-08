# bomb-lab

## 源码解析

这是部分的`bomb.c`的源代码，但是这里只是涉及了一些函数的调用，具体的函数需要自己前往编译好的bomb通过逆向获得汇编代码再步步解析

这里可以看到先是初始化了炸弹`initialize_bomb`，然后获取输入`input`，将输入作为参数传入`phase_1`，应该是根据这个参数发生了某些事情，因此我们需要弄清楚`phase_1`函数里面究竟发生了什么事情

```c
    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");
```

## phase_1

这题比较简单，但是需要知道`gdb`的使用，否则会陷入为什么我知道预设字符串的初始地址但是找不到字符串，将原来的二进制文件逆向出汇编之后，仔细阅读汇编并将汇编转换以下就可以了

#### 原版汇编解读

```assembly
0000000000400ee0 <phase_1>: # 这里是phase_1的汇编代码，调用函数的参数是字符串的地址
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp # 为了调用函数，需要保存寄存器的值
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi # 将0x402400的地址赋给esi，这里其实就是字符串可以从哪里看出
  400ee9:	e8 4a 04 00 00       	call   401338 <strings_not_equal> # 这里函数的参数是字符串的地址，也就是%rsi
  400eee:	85 c0                	test   %eax,%eax # 测试%eax的值是否为0
  400ef0:	74 05                	je     400ef7 <phase_1+0x17> # 如果为0，跳转到400ef7
  400ef2:	e8 43 05 00 00       	call   40143a <explode_bomb> # 如果不为0，调用explode_bomb
  400ef7:	48 83 c4 08          	add    $0x8,%rsp # 恢复栈
  400efb:	c3                   	ret   
```

#### 转换C语言版本

```c
//我们将汇编转化为类似的C语言，但是不保证完全对应
void phase_1(char * input){
    if(strings_not_equal(input,0x402400){
        explode_bomb();
    })//这里的字符串是如何找到的呢
}
```

根据这里我们已经知道是匹配`input`和`0x402400`的字符串，为了找到`0x402400`对应的字符串，我们使用`gdb`来查看内存中地址对应的信息

```shell
gdb bomb #进入对应的debug
x/s 0x402400 #此处打印出0x402400地址所存储的字符串 
```
> ans>>"Border relations with Canada have never been better."

#### 相关函数的汇编解读

这里其实意义不大，因为函数的命名其实看一眼就知道需要做什么，但是由于一开始陷入了深究的陷阱才将这里也详细看了一下

```assembly
000000000040131b <string_length>: # 这里是一个字符串长度的函数，返回值为字符串的长度，使用的是循环来检测末尾的\0来结束检测以获得string长度
  40131b:	80 3f 00             	cmpb   $0x0,(%rdi) # 比较%rdi指向的地址的值是否为0
  40131e:	74 12                	je     401332 <string_length+0x17> # 如果为0，跳转到401332
  401320:	48 89 fa             	mov    %rdi,%rdx # 将%rdi的值赋给%rdx
  401323:	48 83 c2 01          	add    $0x1,%rdx # add指令完成相加的功能，%rdx+1
  401327:	89 d0                	mov    %edx,%eax # 将%edx的值赋给%eax，这里的%edx其实就是%rdx的低32位
  401329:	29 f8                	sub    %edi,%eax # %eax-%edi，%edi是%rdi的低32位，%rdi是传入的参数，%eax是返回值
  40132b:	80 3a 00             	cmpb   $0x0,(%rdx) # 比较0和%rdx指向的地址对应的值，其实这里就是C语言后面的\0来检测字符串的结束
  40132e:	75 f3                	jne    401323 <string_length+0x8> # 如果不相等，跳转到401323
  401330:	f3 c3                	repz ret # 返回
  401332:	b8 00 00 00 00       	mov    $0x0,%eax # 这里是为0跳转的地方，执行将0赋值给%eax，由于%rax是返回的值，这里%eax是%rax的低位，因此高位补0
  401337:	c3                   	ret    

0000000000401338 <strings_not_equal>: # 这个函数是为了比较两个字符串是否相等，返回值为0表示相等，1表示不相等
  401338:	41 54                	push   %r12 # 保存%r12
  40133a:	55                   	push   %rbp # 保存%rbp
  40133b:	53                   	push   %rbx # 保存%rbx
  40133c:	48 89 fb             	mov    %rdi,%rbx # %rdi是第一个参数，将参数赋值给%rbx，根据推断可知第一个参数就是原来的input
  40133f:	48 89 f5             	mov    %rsi,%rbp # %rsi是第二个参数，将参数赋值给%rbp，第二个参数是内定的字符串，字符串的地址从0x402400开始
  401342:	e8 d4 ff ff ff       	call   40131b <string_length> # 调用string_length函数，这里检测的是input的长度
  401347:	41 89 c4             	mov    %eax,%r12d # 将返回值赋值给%r12d，%r12d是%r12的低32位，也即是将input的长度赋值给%r12d
  40134a:	48 89 ef             	mov    %rbp,%rdi # 将第二个参数赋值给%rdi，此时就是将原来设定的字符串地址传给%rdi以便后面检查设定的字符串长度
  40134d:	e8 c9 ff ff ff       	call   40131b <string_length> # 调用string_length函数，这里是检测设定的字符串的长度
  401352:	ba 01 00 00 00       	mov    $0x1,%edx  # 将1赋值给%edx，也就是%rdx的低32位
  401357:	41 39 c4             	cmp    %eax,%r12d # 比较input的长度和设定字符串的长度，%r12d此时是input的长度，%eax是后来一次调用string_length函数的返回值，也就是设定字符串的长度
  40135a:	75 3f                	jne    40139b <strings_not_equal+0x63> # 如果不相等，跳转到40139b
  40135c:	0f b6 03             	movzbl (%rbx),%eax # 将input的第一个字符赋值给%eax
  40135f:	84 c0                	test   %al,%al # 测试%al的值是否为0
  401361:	74 25                	je     401388 <strings_not_equal+0x50> # 如果为0，跳转到401388
  401363:	3a 45 00             	cmp    0x0(%rbp),%al # 比较设定字符串的第一个字符和input的第一个字符
  401366:	74 0a                	je     401372 <strings_not_equal+0x3a> # 如果相等，跳转到401372
  401368:	eb 25                	jmp    40138f <strings_not_equal+0x57> # 如果不相等，跳转到40138f
  40136a:	3a 45 00             	cmp    0x0(%rbp),%al # 比较设定字符串的第一个字符和input的第一个字符
  40136d:	0f 1f 00             	nopl   (%rax) # 空指令
  401370:	75 24                	jne    401396 <strings_not_equal+0x5e> # 如果不相等，跳转到401396
  401372:	48 83 c3 01          	add    $0x1,%rbx # %rbx+1，也就是input的地址+1
  401376:	48 83 c5 01          	add    $0x1,%rbp # %rbp+1，也就是设定字符串的地址+1
  40137a:	0f b6 03             	movzbl (%rbx),%eax # 将input的第一个字符赋值给%eax
  40137d:	84 c0                	test   %al,%al # 测试%al的值是否为0
  40137f:	75 e9                	jne    40136a <strings_not_equal+0x32> # 如果不为0，跳转到40136a，也就是这里是使用一个是否为0来检测是否匹配结束，检测input和设定字符串是否匹配
  401381:	ba 00 00 00 00       	mov    $0x0,%edx # 将0赋值给%edx
  401386:	eb 13                	jmp    40139b <strings_not_equal+0x63> # 跳转到40139b
  401388:	ba 00 00 00 00       	mov    $0x0,%edx # 将0赋值给%edx
  40138d:	eb 0c                	jmp    40139b <strings_not_equal+0x63> # 跳转到40139b
  40138f:	ba 01 00 00 00       	mov    $0x1,%edx
  401394:	eb 05                	jmp    40139b <strings_not_equal+0x63>
  401396:	ba 01 00 00 00       	mov    $0x1,%edx
  40139b:	89 d0                	mov    %edx,%eax
  40139d:	5b                   	pop    %rbx
  40139e:	5d                   	pop    %rbp
  40139f:	41 5c                	pop    %r12
  4013a1:	c3                   	ret    
```

## phase_2

这题从前面解读`phase_2`的汇编代码可以看出出现了循环，并且循环体的内容很抽象，先是解读一下

#### 汇编代码解读

```assembly
0000000000400efc <phase_2>: # 始终记得只有一个参数，就是input，然后参数作为字符串的地址传入，并且是第一个参数记作%rdi
  400efc:	55                   	push   %rbp # 保存%rbp
  400efd:	53                   	push   %rbx # 保存%rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp # 为了调用函数，需要保存寄存器的值
  400f02:	48 89 e6             	mov    %rsp,%rsi # 将%rsp的值赋给%rsi，作为参数作为下一个函数
  400f05:	e8 52 05 00 00       	call   40145c <read_six_numbers> # 使用两个参数来调用函数%rdi作为input，%rsi使用栈地址
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp) # 比较%rsp的值和0x1
  400f0e:	74 20                	je     400f30 <phase_2+0x34> # 如果%rsp的值为0x1，跳转到400f30
  400f10:	e8 25 05 00 00       	call   40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	call   40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx # 将%rsp的值加上0x4赋给%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp # 将%rsp的值加上0x18赋给%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b> # 跳转到400f17
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	ret    
```

#### 转换C语言版本

```c
phase_2(char *input) {
    %rdi = char * input;
    %rsi = %rsp;
    read_six_numbers(char * input,%rsi);//根据函数的名字可知需要查看六个数字,这里获取了一个返回值
    //通过阅读 read_six_number 汇编代码，可以直到 read_six_number 是把字符串转为6个数字，存储在整数数组中。
    //也就是将输入的字符串通过阅读前面6个字符，并返回一个数组值存储在栈上

    if(*%rsp==1){
        //goto : 400f30;//栈指针在传入之后被修改了，所以这里必须要要%rsp==1否则就爆炸，回去看函数read_six_numbers如何调整%rsp的
        //这里*%rsp-->返回的数组的第一位arr[0]
        %rbx = %rsp + 4; //栈的位置,也就是第二个
        %rbp = %rsp + 24; //栈的位置，也就是最后一个
        goto : 400f17; 
    }

    explode_bomb();// 这里需要避开
    goto : 400f30;

    400f17:{
        %eax = *(%rbx - 0x4);//也就是第一个位置的数组的值，但是此时%rsp指向的是第二个
        %eax = %eax  + %eax ;//第一个乘上2
        if(%eax==*%rbx){
            goto : 400f25;//如果此时数组的值等于前一个的值（也就是乘上2之后的），循环继续
        }
        explode_bomb(); // 这里需要避开
    }


    400f25:{
        %rbx = %rbx + 0x4;//此时的数组index+1
        if(%rbp != %rbx){
            goto : 400f17;//如果数组index还没有触及最后一个数组的地址，循环继续
        }
        goto : 400f3c;
    }
	
    //这段代码已经融合入前方，因此后续不会再出现了
    400f30:{
        %rbx = %rsp + 4;
        %rbp = %rsp + 24;
        goto : 400f17;
    }
	
    //这就是退出出口了
    400f3c:{
        %rsp = %rsp + 0x28;
        return;
    }
    
}
```

解读这个转换C语言版本的代码之后，我们会发现，read_six_numbers函数就是观察出6个输入的数字，因此输入的时候需要隔开！然后很关键的一点就是数字可以多于6个但是不能少于6个！！！`input_numbers>=6`

回到`phase_2`就是发现这里需要回来的数组里面，`arr[0]==1`然后`arr[i]=arr[i-1]*2`就是这样的两个条件就可以避免掉所有的炸弹引爆，也就是这一段

```c
400f17:{
    %eax = *(%rbx - 0x4);//也就是第一个位置的数组的值，但是此时%rsp指向的是第二个
    %eax = %eax  + %eax ;//第一个乘上2
    if(%eax==*%rbx){
        goto : 400f25;//如果此时数组的值等于前一个的值（也就是乘上2之后的），循环继续
    }
    explode_bomb(); // 这里需要避开
}
//将上面的代码转换为好看一点的
400f17{
    arr[i-1] += arr[i-1]
    if(arr[i]==arr[i-1]){
        //继续循环！！！
    }
}
```

那么很明朗这个答案，也就是以1为首项，q=2的等比数列

> 1 2 4 8 16 32 (if you want you can add 64.........)

#### read_six_numbers汇编解读

```assembly
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp # 日常分配栈空间24个字节，也就是6个int
  401460:	48 89 f2             	mov    %rsi,%rdx # 将第二个参数%rsi赋值给%rdx
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx # 这里使用了lea %rcx=(%rsi)+4
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax # 这里使用了lea %rax=(%rsi)+20
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp) # 将%rax的值赋值给0x8(%rsp)=(%rsp)+8的位置
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax # 这里使用了lea %rax=(%rsi)+16,就是第五个分配的栈对应的地址
  401474:	48 89 04 24          	mov    %rax,(%rsp) # 将%rax的值赋值给(%rsp)
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9 # 这里使用了lea %r9=(%rsi)+12
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8 # 这里使用了lea %r8=(%rsi)+8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi # 将0x4025c3赋值给%esi
  401485:	b8 00 00 00 00       	mov    $0x0,%eax # 将0赋值给%eax
  40148a:	e8 61 f7 ff ff       	call   400bf0 <__isoc99_sscanf@plt>
  40148f:	83 f8 05             	cmp    $0x5,%eax # 比较%eax和5
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d> # 如果大于5，跳转到401499，也就是不爆炸
  401494:	e8 a1 ff ff ff       	call   40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp # 释放栈空间
  40149d:	c3                   	ret    # 返回、
```

## phase_3
