.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
.global irq128

irq0:
    cli 
    push $0
    push $32
    jmp irq_handle

irq1:
    cli
    push $1
    push $33
    jmp irq_handle

irq2:
    cli
    push $2
    push $34
    jmp irq_handle

irq3:
    cli
    push $3
    push $35
    jmp irq_handle

irq4:
    cli
    push $4
    push $36
    jmp irq_handle

irq5:
    cli
    push $5
    push $37
    jmp irq_handle

irq6:
    cli
    push $6
    push $38
    jmp irq_handle

irq7:
    cli
    push $7
    push $39
    jmp irq_handle

irq8:
    cli
    push $8
    push $40
    jmp irq_handle

irq9:
    cli
    push $9
    push $41
    jmp irq_handle

irq10:
    cli
    push $10
    push $42
    jmp irq_handle

irq11:
    cli
    push $11
    push $43
    jmp irq_handle

irq12:
    cli
    push $12
    push $44
    jmp irq_handle

irq13:
    cli
    push $13
    push $45
    jmp irq_handle

irq14:
    cli
    push $14
    push $46
    jmp irq_handle

irq15:
    cli
    push $15
    push $47
    jmp irq_handle

irq128:
    cli
    push $0x80
    push $0x80
    jmp irq_handle

irq_handle:
    pusha                  
    push %ds                
    push %es                
    push %fs                
    push %gs                
    mov $0x10, %ax         
    mov %ax, %ds            
    mov %ax, %es            
    mov %ax, %fs            
    mov %ax, %gs            
    movl %esp, %eax         
    push %eax               
    movl $irq_handler, %eax 
    call *%eax              
    pop %eax                
    pop %gs                 
    pop %fs                 
    pop %es                 
    pop %ds                 
    popal                   
    addl $8, %esp           
    iret                    
