//
// Created by alex on 16.04.25.
//
#pragma once
#include "config.h"
#ifndef SANDBOX_H
#define SANDBOX_H
pid_t sandbox_create( struct sandbox_config *cfg);
 void child_setup( struct sandbox_config *cfg);

 void setup_namespaces( struct sandbox_config *cfg) ;


 void enter_chroot( struct sandbox_config *cfg);
#endif //SANDBOX_H
