//
// Created by alex on 16.04.25.
//

#ifndef SANDBOX_H
#define SANDBOX_H
pid_t sandbox_create(const struct sandbox_config *cfg);
static void child_setup(const struct sandbox_config *cfg);

static void setup_namespaces(const struct sandbox_config *cfg) ;


static void enter_chroot(const char *new_root);
#endif //SANDBOX_H
