#include <osmpbf/abstractnodeinputadaptor.h>
#include <iostream>

namespace osmpbf {

void AbstractNodeInputAdaptor::print(std::ostream& out) const
{
	out << "{\n";
	printCommon(out);
	out << "\tlat:" << this->latd() << '\n';
	out << "\tlon:" << this->lond() << '\n';
	out << "}";
}


}//end namespace osmpbf
