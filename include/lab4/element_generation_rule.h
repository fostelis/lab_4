#ifndef ELEMENT_GENERATION_RULE_H
#define ELEMENT_GENERATION_RULE_H

#include <memory>
#include "ordinal_number.h"
#include "ordinal_operations.h"



template<class T>
class element_generation_rule {
public:
    virtual ~element_generation_rule() = default;
    virtual T get(const ordinal_number& index) const = 0;
    virtual ordinal_number length() const = 0;
    virtual std::shared_ptr<element_generation_rule<T>> clone_rule() const = 0;
};

#endif
