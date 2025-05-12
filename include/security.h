//
// Created by alex on 16.04.25.
//

#ifndef SECURITY_H
#define SECURITY_H
void security_apply_rlimits(const struct sandbox_config *cfg);
void security_drop_caps(const struct sandbox_config *cfg);
#endif //SECURITY_H
