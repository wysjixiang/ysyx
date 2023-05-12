/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

enum{
	wp_pc =1,
	wp_gpr,
	wp_addr,
};

#define MAX_WP_EXPRE 30

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *pre;
  /* TODO: Add more members if necessary */
	word_t pre_value;
	word_t current_value;
	bool occupy;
	char expression[MAX_WP_EXPRE];

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static WP *tail = NULL;

// init watch point
// already init in sdb.c
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
		wp_pool[i].pre = ( i == 0 ? NULL	:	&wp_pool[i-1]);
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].occupy = 0;
  }
	tail = NULL;
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	if(free_ == NULL){
		printf("The space:%d for wp_pool is full\n",NR_WP);
		return NULL;
	}	else{
		if(head == NULL){
			head = free_;
			tail = free_;
			free_ = free_->next;
			//free_->pre = NULL;
			head->next = NULL;
			head->pre = NULL;
		}	else{
			free_->pre = tail;
			tail->next = free_;
			tail = free_;
			free_ = free_->next;
			tail->next = NULL;
		}
		return tail;
	}
}

bool free_wp(WP *p){
	if(head == NULL){
		printf("There is no watch point set\n");
		return 0;
	}	else{
		int id = p->occupy;
		if(id == 0){
			printf("This space is free, no watch point\n");
			return 0;
			// if only one element
		}	else if(head == tail){
			head->next = free_;
			free_->pre = head;
			free_ = head;
			head = NULL;
			tail = NULL;
			// more than 1 element
		}	else{
			free_->pre = p;
			if(p == tail){
				tail = tail->pre;
				tail->next = NULL;
				p->next = free_;
				free_ = p;
			}	else if(p == head){
				head = head->next;
				head ->pre = NULL;
				p->next = free_;
				free_ = p;
			}	else{
				p->pre->next = p->next;
				p->next->pre = p->pre;
				p->next = free_;
				free_ = p;
			}
		}
	}

p->occupy = 0;

return true;

}


bool AddWp(char *str){

	WP* p = new_wp();
	if(p == NULL){
		return false;
	}	else{
		int i =0;
		for(i=0;str[i] != '\0';i++){
			p->expression[i] = str[i];
		}
		p->expression[i] = '\0';
		bool success = 0;
		word_t ret;
		ret = expr(p->expression, &success);
		if(success == 0){
			printf("expression analysis failed\n");
			assert(0);
		}
		p->current_value = ret;
		p->pre_value = ret;
	}
	p->occupy = 1;
	return true;
}


bool DelWp(char *args){
	word_t id;
	bool success = 0;
	id = expr(args,&success);
	if(success == 0){
		printf("Del Wp analysis args failed\n'");
		return false;
	}

	if(id < 0 || id > NR_WP-1 ){
		printf("Invalid wp id:%lu. please retype\n",id);
		return false;
	}	else{
		return free_wp(&wp_pool[id]);
	}
}


void ShowWp(){
	WP *p = head;
	if(p == NULL){
		printf("There is no wp right now, no info to print\n");
		return ;
	}
	while(p != NULL){
		printf("WP ID: %02d; pre_value = %lu, current_value = %lu; expression = %s\n",p->NO,p->pre_value,p->current_value,p->expression);
		p = p->next;
	}
	return;
}

void UpdateWp(){
	WP *p = head;
	if(p == NULL){
		return;
	}else{
		while(p != NULL){
			bool success = 0;
			p->pre_value = p->current_value;
			p->current_value = expr(p->expression, &success);
			if(success == 0){
				printf("Error when update Wp-%d\n",p->NO);
			}
			if(p->current_value != p->pre_value){
				printf("WP ID:%d status changed! pre_value = %lu, current_value = %lu; Expression = %s\n",p->NO,
				p->pre_value,p->current_value,p->expression);
				if(!(nemu_state.state == NEMU_END || nemu_state.state == NEMU_ABORT))
					nemu_state.state = NEMU_STOP;
			}
			p = p->next;
		}
	}
	return;
}