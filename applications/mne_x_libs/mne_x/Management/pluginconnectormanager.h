//=============================================================================================================
/**
* @file     pluginconnectomanager.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     August, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains declaration of PluginConnectorManager class.
*
*/

#ifndef PLUGINCONNECTORMANAGER_H
#define PLUGINCONNECTORMANAGER_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "pluginconnectorconnection.h"


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QObject>
#include <QSharedPointer>
#include <QList>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE MNEX
//=============================================================================================================

namespace MNEX
{

//=========================================================================================================
/**
* PluginConnectorManager manages connections between connectors.
*
* @brief The PluginConnectorManager class manages the connections of a set of plugins.
*/
class PluginConnectorManager : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<PluginConnectorManager> SPtr;            /**< Shared pointer type for PluginConnectorManager. */
    typedef QSharedPointer<const PluginConnectorManager> ConstSPtr; /**< Const shared pointer type for PluginConnectorManager. */

    typedef QList<PluginConnectorConnection::SPtr> PluginConnectorConnectionList;   /**< Shared pointer type for PluginConnectorConnection::SPtr. */


    //=========================================================================================================
    /**
    * Constructs a PluginConnectorManager.
    */
    explicit PluginConnectorManager(QObject *parent = 0);
    
signals:


private:
    PluginConnectorConnectionList m_conConList;     /**< Lit of connector connections. */



};

} //Namespace

#endif // PLUGINCONNECTORMANAGER_H
