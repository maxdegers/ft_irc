#ifndef PASS_HPP
#define PASS_HPP

#include "ACommand.hpp"

class Pass : public ACommand
{
private:
    
public:
    Pass( void );
    Pass( const Pass &src );
    Pass &operator=( const Pass &src );
    
    ~Pass();
    
};

#endif // PASS_HPP
