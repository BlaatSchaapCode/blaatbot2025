/*
 * PluginLoadable.hpp
 *
 *  Created on: 9 mei 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADABLE_HPP_
#define SRC_PLUGINLOADABLE_HPP_

// Third Party libraries
#include <nlohmann/json.hpp>

namespace geblaat {

class PluginLoadable {
public:
	virtual int setConfig(nlohmann::json) = 0;
};

};



#endif /* SRC_PLUGINLOADABLE_HPP_ */
