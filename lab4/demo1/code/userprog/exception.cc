// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include <iostream>
#include "system.h"
#include "syscall.h"

#ifndef SpaceId
  #define SpaceId int
#endif
int new_Exec(char *name);
void new_Exit(int status);
int new_Join(int id);
//extern void new_Exit(int exitCode);
//extern int new_Join(int id);
//extern int new_Exec(char *name);

using namespace std;

#define MAXLEN 128
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);//ȡ��ϵͳ���ô���
    int i, tmp, spaceId, from, id, exitCode, Status;
    char *fileName = new char[MAXLEN + 1];

    if ((which == SyscallException)) {//SyscallException��Ϊϵͳ����,��machine.h���ж���
        switch (type) {
            case SC_Halt :   
  	        DEBUG('a', "Shutdown, initiated by user program.\n");
     	        interrupt->Halt();
                pM->AdjustPCRegs();
                break;
            case SC_Exec :
                DEBUG('a', "Exec call\n");
                from = machine->ReadRegister(4);//????Ϊʲô��4
               
                for (i = 0; i < MAXLEN; i++) {//���ļ�������fileName[]��,Ϊʲô����ÿ�ζ��ڴ�һ���ֽ�
                    machine->ReadMem(from, 1, &tmp);
                    fileName[i] = (char)tmp;
                    if ((char)tmp == '\0')
                        break;
                    from++;
                } 
                   
                if (i == MAXLEN) 
                    cout << "the file name must be less than 128 chars!"
                         << endl;
                
                spaceId = new_Exec(fileName);//new_Exec()�д�����fileName����Ҫ�õ��ļ���,��������,spaceIdҪ���淵��
                machine->WriteRegister(2, spaceId);//�Ĵ���2�淵��ֵ
                pM->AdjustPCRegs();
                break;
            case SC_Exit :
                DEBUG('a', "Exit call\n");
                Status = machine->ReadRegister(4);//new_Exit()�д�����Status��ֻ��ӼĴ�����ȡ��״̬��ż���,��ΪʲôҲ��4?
                new_Exit(Status);
                
		pM->AdjustPCRegs();
                break;
            case SC_Join :
                DEBUG('a', "Join call\n");
                id = machine->ReadRegister(4);
                  
                exitCode = new_Join(id);
                machine->WriteRegister(2, exitCode);
		
                pM->AdjustPCRegs();
                break;   
	    case SC_Yield :
		DEBUG('a', "Yield call, just to call cout function\n");		
		cout << endl << "   test running" <<endl;
                pM->AdjustPCRegs();
		break;
            defualt :              
	        printf("we do not support this exception %d %d\n", which, type);
	        ASSERT(FALSE);
        }
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
