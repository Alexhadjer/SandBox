//
// Created by alex on 16.04.25.
//
#pragma once //forgot why we need it here  too afraid to remove
#include "config.h"
#ifndef SANDBOX_H
#define SANDBOX_H
pid_t sandbox_create( struct sandbox_config *cfg);
 void child_setup( struct sandbox_config *cfg);

 void setup_namespaces( struct sandbox_config *cfg) ;


 void enter_chroot( struct sandbox_config *cfg);
 int launch_shell(struct sandbox_config *cfg);
void drop_privileges(struct sandbox_config *cfg);
void new_environ(int new_env);
#endif //SANDBOX_H
