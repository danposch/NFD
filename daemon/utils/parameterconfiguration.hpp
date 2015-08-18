/**
 * Copyright (c) 2015 Daniel Posch (Alpen-Adria Universität Klagenfurt)
 *
 * This file is part of the ndnSIM extension for Stochastic Adaptive Forwarding (SAF).
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef PARAMETERCONFIGURATION_H
#define PARAMETERCONFIGURATION_H

#include <cstddef>
#include <map>
#include <string>

//some additional defines
#define DROP_FACE_ID -1
#define FACE_NOT_FOUND -1

/**
 * @brief The ParameterConfiguration class is used to set/get parameters to configure SAF.
 * The class uses a singleton pattern.
 */
class ParameterConfiguration
{
public:

  /**
   * @brief returns the singleton instance.
   * @return
   */
  static ParameterConfiguration* getInstance();

  /**
   * @brief sets a parameter
   * @param param_name the name of the parameter.
   * @param value the value of the parameter.
   */
  void setParameter(std::string param_name, double value);

  /**
   * @brief gets a parameter.
   * @param para_name the name of the parameter.
   * @return
   */
  double getParameter(std::string para_name);

protected:  
  ParameterConfiguration();

  static ParameterConfiguration* instance;

  std::map<
  std::string /*param name*/,
  double /*param value*/
  > typedef ParameterMap;

  ParameterMap pmap;
};

#endif // PARAMETERCONFIGURATION_H
