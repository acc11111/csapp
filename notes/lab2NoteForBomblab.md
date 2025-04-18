# bomb-lab

## 源码解析

这是部分的`bomb.c`的源代码，但是这里只是涉及了一些函数的调用，具体的函数需要自己前往编译好的bomb通过逆向获得汇编代码再步步解析

这里可以看到先是初始化了炸弹`initialize_bomb`，然后获取输入`input`，将输入作为参数传入`phase_1`，应该是根据这个参数发生了某些事情，因此我们需要弄清楚`phase_X`函数里面究竟发生了什么事情

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

这题需要`题1`的`gdb`调试以及对条件跳转知识的熟悉

#### 汇编代码解读

```assembly
0000000000400f43 <phase_3>: # input in %rdi
  400f43:	48 83 ec 18          	sub    $0x18,%rsp  
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx  # rcx = *(rsp + 12) --> 这是第四个参数
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx  # rdx = *(rsp + 8) --> 这里是为了作为sscanf的参数，这是第三个参数
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi  # esi = 0x4025cf -->这里其实未知具体是什么，可以使用gdb查看运行时对应的内存获取，获取可知是%d %d,所以需要输入超过两个数字，并且空格隔开
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	call   400bf0 <__isoc99_sscanf@plt> # sscanf(rdi, 0x4025cf, rsp + 8, rsp + 12) -->第一个参数是src/也就是input，第二个参数是esi指向的字符串，第三/四个参数就是接受的地方，因此rsp+8存储num1，rsp+12存储num2
  400f60:	83 f8 01             	cmp    $0x1,%eax # 这里比较一下sscanf的返回值，返回值是读取的个数，也就是需要大于1的个数
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	call   40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)  # 比较num1和0x7
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a> # 如果num1大于0x7，跳转到400fad，也就是爆炸，所以num1要小于等于7
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax # eax = num1
  400f75:	ff 24 c5 70 24 40 00 	jmp    *0x402470(,%rax,8) # 跳转到0x402470 + 8 * num1,根据查询这些的地址发现是下列的地址，编号如下
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax # 0
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax # 2
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax # 3
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax # 4
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax # 5
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax # 6
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax # 7
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	call   40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax # 1
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax # 比较num2和eax，eax需要根据num1跳转之后赋值，因此num2和num1的值息息相关，本题一共有0-7共计8个答案
  400fc2:	74 05                	je     400fc9 <phase_3+0x86> # num2对应上num1跳转的值就不引发爆炸
  400fc4:	e8 71 04 00 00       	call   40143a <explode_bomb> 
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	ret   
```

#### 转换C语言版本

这题转换C语言比较累，要写好多地址，直接一步到位的C语言版本得了

```c
phase_3(char * input){
    sscanf(input,"%d %d",num1,num2);
    if(num1 > 7){
        bomb();
    }
    switch(num1){
        case 0: 
            if(num2 != 0xcf){
                bomb();
            }
            break;
        case 1: 
            if(num2 != 0x137){
                bomb();
            }
            break;
        case 2: 
            if(num2 != 0x2c3){
                bomb();
            }
            break;
        case 3: 
            if(num2 != 0x100){
                bomb();
            }
            break;
        case 4: 
            if(num2 != 0x185){
                bomb();
            }
            break;
        case 5: 
            if(num2 != 0xcc){
                bomb();
            }
            break;
        case 6: 
            if(num2 != 0x200){
                bomb();
            }
            break;
        case 7: 
            if(num2 != 0x147){
                bomb();
            }
            break;
        default:
            bomb();
    }
    return ;
}
```

根据这个C语言转换之后可以看出，需要num1和num2之间有对应关系，并且num1的取值是0-7其中一个整数

> 可能的答案有八种，分别是
>
> 0 - 0xcf / 207	
> 
> 1 - 0x137 / 311 	
> 
> 2 - 0x2c3 / 707 	
> 
> 3 - 0x100 / 256 	
> 
> 4 - 0x185 / 389 
> 
> 5 - 0xcc / 204 	
> 
> 6 - 0x200 / 512 	
> 
> 7 - 0x147 / 327 

#### sscanf的补充

```c
    //sscanf(rdi, 0x4025cf, rsp + 8, rsp + 12) -->第一个参数是src/也就是input，第二个参数是esi指向的字符串，第三/四个参数就是接受的地方，因此rsp+8存储num1，rsp+12存储num2
	//sscanf(src,format,...)后面的...是可变参数，指的是接收的位置
```

## phase_4

第四题考察了一个函数的递归，但是可能是故意的，给我们留了一条不需要递归的路，这就很有生活了，只需要顺着我们想要的结果观察那个递归就可以一次跳出，下面来看一下

#### 汇编解读

```assembly
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi # 到这里其实很熟悉了，和前面的题3是差不多的，也就是0x4025cf是%d %d 
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax # eax = 0
  401024:	e8 c7 fb ff ff       	call   400bf0 <__isoc99_sscanf@plt> # 从input读取两个数字
  401029:	83 f8 02             	cmp    $0x2,%eax  # 比较读取的个数是否为2
  40102c:	75 07                	jne    401035 <phase_4+0x29> # 如果不是2个数字，跳转到401035，也即是爆炸，因此只能读取两个数字
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)  # 比较num1是否为0xe/14
  401033:	76 05                	jbe    40103a <phase_4+0x2e> # 如果num1小于等于0xe/14，跳转到40103a，这里一定要小于等于14
  401035:	e8 00 04 00 00       	call   40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx # edx = 14第三个参数
  40103f:	be 00 00 00 00       	mov    $0x0,%esi # esi = 0第二个参数
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi # edi = num1第一个参数
  401048:	e8 81 ff ff ff       	call   400fce <func4> # 在这里调用函数func4，传入上面的这几个参数
  40104d:	85 c0                	test   %eax,%eax # 这里检查返回值是不是0
  40104f:	75 07                	jne    401058 <phase_4+0x4c> # 如果不是0，就爆炸，也就搞懂了func4需要是返回0
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp) # num2也必须0
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	call   40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	ret  
```

在这里可以看出，从`sscanf`这里读取两个数字，前置的操作与`phase_3`比较相似，这里不过多解释。然后是在传入了三个参数给`func4(num1,0,14)`，然后需要返回值`eax`是0，同时需要`num2`是0，那么就很明确了，`num2=0`，然后需要进入`func4`找到`eax=0`的出口条件

```assembly
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp # 为了调用函数，需要保存寄存器的值，影响不大
  400fd2:	89 d0                	mov    %edx,%eax # 第三个参数赋给eax，eax = edx = 14
  400fd4:	29 f0                	sub    %esi,%eax # esi = 0，所以eax = 14
  400fd6:	89 c1                	mov    %eax,%ecx # ecx = num1,所以是ecx = num1 - eax = num1 - 14
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx # ecx >>= 31，也就是ecx = 0,因为这里是逻辑右移
  400fdb:	01 c8                	add    %ecx,%eax # eax += ecx，也就是eax = eax + 0 = 14
  400fdd:	d1 f8                	sar    %eax # eax >>= 1，也就是eax = 14 / 2 = 7
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx # ecx = rax + rsi = 7
  400fe2:	39 f9                	cmp    %edi,%ecx 
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24> # if(num1 <= 7) goto 400ff2
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx # edx = rcx - 1 = num1 / 2 - 1
  400fe9:	e8 e0 ff ff ff       	call   400fce <func4>
  400fee:	01 c0                	add    %eax,%eax
  400ff0:	eb 15                	jmp    401007 <func4+0x39>
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax # eax = 0
  400ff7:	39 f9                	cmp    %edi,%ecx # 比较edi和ecx,也就是num1 和 7比较
  400ff9:	7d 0c                	jge    401007 <func4+0x39> # if(num1 >= 7 ) goto 401007
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi # esi = rcx + 1 = 7 + 1 = 8
  400ffe:	e8 cb ff ff ff       	call   400fce <func4> # 调用func4,此时三个参数分别为num1 / 2 + 1, 8, 14
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401007:	48 83 c4 08          	add    $0x8,%rsp # 恢复栈
  40100b:	c3                   	ret    
```

在这里，找到了一个很巧妙的地方，`400ff2`处进行`eax=0`，这里正是我们想要的，观察能不能出来即可。

#### 转换C语言版本

此处不再转换`phase_4`的汇编，因为比较清晰了，直接转换`func4`的汇编代码即可解除炸弹，并且直接展示了第一次传入参数的情况，因为第一次即可跳出递归

```c
phase_4(num1,0,14){
    //此处通过一通操作运算下来的结果是
    eax = 7;
    ecx = 7;
    if(num1 <= ecx){
        //num1 <= 7
        eax = 0;
        if(num1 >= ecx){
            //num1 >= 7
            return;
        }else{
            esi = rcx + 1;
            func4(rdi,rsi,rdx);//func4(num1,8,14)
        }else{
            edx = rcx -1;
            func4(....);
            eax = eax + eax;
            return;
        }
    }
}
```

根据这个比较分析可以得出，需要`num1=7`既可一次跳出递归，应该是还有其他答案。

#### ANS

综上，本题与第三题比较类似，但是这题还是留了一个巧妙的地方对我来说，只需要自己可以从中那个自己想要的结果不断满足（~~舔~~）他跳出的条件就可以出来

> 一种可能的答案
>
> 7 0
>
> 后续有机会会回来补全剩下的答案，待开发...

## phase_5

`ASCII码+char数组遍历+位操作知识+循环`这里真的是有点逆天，将前面很多东西都拿到了这里

#### 汇编解读

这次没有直接在汇编上面写，而是转换了一下，自己顺着汇编写了第一次查询，然后发现后续都是差不多的操作只是索引不同

```c
input in % rdi

分配32字节的空间

rbx = input // 指向的是地址

rsp + 16 + 8 = 24字节的地方存放的rax的值，也就是保护值

也就是24 - 32字节存放的是保护值，目前还没有进入逻辑

//rsp + 16 17 18 19 20 21 ----- 22 23 24从后面来看，我们需要给这些位置赋值，那么就是将这些位置的
//并且这个题很逆天的就是，他有很多的字符，但是你只能使用前面的几个，否则被截断到16个字符你就难受了
//综合来看就是为"flyers"需要在"maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"里面找到，然后我们输入的就是后者字符串的索引
//注意我们只能输入字符，不能输入数字，因此这里还涉及到ascii的转换
//索引就是 9 15 14 5 6 7
//那么我们输入的低四位一定是这样的，但是字符是8bits组成
eax = 0;

string_length(input){
    return len

}

if(len != 6){
    bomb(); //因此输入的数据必须是6个长度字符
}else if(len ==6){
    goto 4010d2:{
        eax = 0;
        goto 40108b:{
            //通过0扩展将内存byte移动到long长度的寄存器
            ecx = *(rbx + rax) = *(input + 0) = input[0];
            //也就是上面的一步拿出的是第一个字符ecx = input[0];
            //*(rsp) = cl; //cl就是rcx的long部分，那其实很有意思了，直接就是将这个字符放过去啊
            *(rsp) = input[0];
            //rdx =*rsp;
            rdx = input[0];
            //edx &= 0xf;这里其实就是取低的4位
            edx 取低四位；
            //edx = *(rdx + 0x4024b0)//注意这里的rdx在前面被真实过了，所以就是取低四位的字符作为无符号数来和这个相加；
            edx = *(input[0]取低四位 + 0x4024b0);
            //*(rsp + rax + 16) = rdx的低8位/一个字节;
            *(rsp + 16) = rdx的低8位;也就是一个字符吧我认为
            rax += 1; //这里我有一个预感就是需要循环上面的东西，很明显我们上面简化了一些东西都是因为rax在这里是0；
            if(rax != 6){
                goto 40108b:{};

            }else{
                *(rsp + 22) = 0; //置0模拟停止符号
                esi = $0x40245e;
                rdi = rsp + 16
                //注意这两个参数传入了string_not_equal里面
                eax = strings_not_equal(rsp + 16; 0x40245e);//比较这两个字符串
                if(eax == 0){
                    //检查栈是否被破坏
                    if(栈被破坏就退出){
                        退出；
                    }
                }
            }


        }
    }

}


```

通过里面可以发现，在内存中储存了两个字符串，分别如下

- `"flyers"`
- `"maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"`

但是不要看第二个字符串很长，这是陷阱，我们只能取0-15的字符，在与操作那里限制了。主要来说就是在长的字符串里面取出字符，并且按照上面短字符串的顺序来排序存到内存中就万事大吉

#### 转换C语言

其实上面的就很C语言了，这里只是简化版

```c
if(input.length != 6){
    bomb();
}
char saved_str[6];
for(int i = 0;i < 6; i++){
    saved_str[i] = *(0x4024b0 + (input[i]&0xf));
}
if(saved_str == "flyers"){
    return 0;
}
```

其实就是这么简单的事情，然后我们找到的下标就是这几个`9 15 14 5 6 7`。然后根据这些后四位为这几个数字的ASCII字符查找，我们询问GPT给出了以下字符

> 我需要找到6个字符，它们的ASCII值低4位分别为9, 15, 14, 5, 6, 7。
>
> 以下是可能的字符选择：
>
> 1. **低4位=9 (十六进制0x9)的字符**：
>    - `i` (ASCII 105 = 0x69)
>    - `I` (ASCII 73 = 0x49)
>    - `)` (ASCII 41 = 0x29)
>    - `9` (ASCII 57 = 0x39)
>
> 2. **低4位=15 (十六进制0xF)的字符**：
>    - `o` (ASCII 111 = 0x6F)
>    - `O` (ASCII 79 = 0x4F)
>    - `_` (ASCII 95 = 0x5F)
>    - `/` (ASCII 47 = 0x2F)
>
> 3. **低4位=14 (十六进制0xE)的字符**：
>    - `n` (ASCII 110 = 0x6E)
>    - `N` (ASCII 78 = 0x4E)
>    - `.` (ASCII 46 = 0x2E)
>    - `>` (ASCII 62 = 0x3E)
>
> 4. **低4位=5 (十六进制0x5)的字符**：
>    - `e` (ASCII 101 = 0x65)
>    - `E` (ASCII 69 = 0x45)
>    - `%` (ASCII 37 = 0x25)
>    - `5` (ASCII 53 = 0x35)
>
> 5. **低4位=6 (十六进制0x6)的字符**：
>    - `f` (ASCII 102 = 0x66)
>    - `F` (ASCII 70 = 0x46)
>    - `&` (ASCII 38 = 0x26)
>    - `6` (ASCII 54 = 0x36)
>
> 6. **低4位=7 (十六进制0x7)的字符**：
>    - `g` (ASCII 103 = 0x67)
>    - `G` (ASCII 71 = 0x47)
>    - `'` (ASCII 39 = 0x27)
>    - `7` (ASCII 55 = 0x37)
>
> 最简单的解决方案是使用全小写字母：`ionefg`或全大写字母：`IONEFG`

也就是在这里排列组合都是可以的答案，所以这里我们选择他给出的推荐答案即可。

#### ANS

>  最简单的解决方案是使用全小写字母：`ionefg`或全大写字母：`IONEFG`

## phase_6

终于也是来到了最后一个,三天打渔两天晒网确实是慢!

但是这个设计到了链表的内容,我觉得可以留到后面数据结构学完了再回来完成这题

## secret_phase

没想到吧,拆完六个还有一个隐藏彩蛋,但是这个非常难,简单看了一下是树方面的内容,所以和[phase_6](## phase_6)一样后面再回来完成,这里先告一段落!

## ANS-SUMMARY

答案汇总一下,便于查找

1. "Border relations with Canada have never been better."

2. 1 2 4 8 16 32 (if you want you can add 64.........)

3. 可能的答案有八种，分别是

   0 - 0xcf / 207	

   1 - 0x137 / 311 	

   2 - 0x2c3 / 707 	

   3 - 0x100 / 256 	

   4 - 0x185 / 389 

   5 - 0xcc / 204 	

   6 - 0x200 / 512 	

   7 - 0x147 / 327 

4. 一种可能的答案

   7 0

   后续有机会会回来补全剩下的答案，待开发...

5. 最简单的解决方案是使用全小写字母：`ionefg`或全大写字母：`IONEFG`

6. 4 3 2 1 6 5p
