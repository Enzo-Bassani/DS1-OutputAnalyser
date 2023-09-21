/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Buffer.cpp
 * Author: rafael.luiz.cancian
 * 
 * Created on
 */

#include "Buffer.h"
#include "../../kernel/simulator/Model.h"
#include "../../kernel/simulator/Simulator.h"
#include "../../kernel/simulator/PluginManager.h"

#ifdef PLUGINCONNECT_DYNAMIC

extern "C" StaticGetPluginInformation GetPluginInformation() {
	return &Buffer::GetPluginInformation;
}
#endif

ModelDataDefinition* Buffer::NewInstance(Model* model, std::string name) {
	return new Buffer(model, name);
}

Buffer::Buffer(Model* model, std::string name) : ModelComponent(model, Util::TypeOf<Buffer>(), name) {
}

std::string Buffer::show() {
	return ModelComponent::show() + "";
}

// public static 

ModelComponent* Buffer::LoadInstance(Model* model, PersistenceRecord *fields) {
	Buffer* newComponent = new Buffer(model);
	try {
		newComponent->_loadInstance(fields);
	} catch (const std::exception& e) {

	}
	return newComponent;
}

PluginInformation* Buffer::GetPluginInformation() {
	PluginInformation* info = new PluginInformation(Util::TypeOf<Buffer>(), &Buffer::LoadInstance, &Buffer::NewInstance);
	info->setDescriptionHelp("//@TODO");
	info->insertDynamicLibFileDependence("queue.so");
	info->insertDynamicLibFileDependence("signaldata.so");
	return info;
}

// protected virtual -- must be overriden

void Buffer::_onDispatchEvent(Entity* entity, unsigned int inputPortNumber) {
	if (_buffer->at(_capacity-1) != nullptr) { // full buffer
		trace("Entity arrived on a full buffer");
		switch (_arrivalOnFullBufferRule) {
			case ArrivalOnFullBufferRule::Dispose:

				break;
			case ArrivalOnFullBufferRule::ReplaceLastPosition:

				break;
			case ArrivalOnFullBufferRule::SendToBulkPort:

				break;
		}
	}
}

bool Buffer::_loadInstance(PersistenceRecord *fields) {
	bool res = ModelComponent::_loadInstance(fields);
	if (res) {
		// @TODO: not implemented yet
	}
	return res;
}

void Buffer::_saveInstance(PersistenceRecord *fields, bool saveDefaultValues) {
	ModelComponent::_saveInstance(fields, saveDefaultValues);
	// @TODO: not implemented yet
}


// protected virtual -- could be overriden 

//ParserChangesInformation* DummyElement::_getParserChangesInformation() {}

bool Buffer::_check(std::string* errorMessage) {
	bool resultAll = true;
	//...
	return resultAll;
}

ParserChangesInformation* Buffer::_getParserChangesInformation() {
	ParserChangesInformation* changes = new ParserChangesInformation();
	//@TODO not implemented yet
	//changes->getProductionToAdd()->insert(...);
	//changes->getTokensToAdd()->insert(...);
	return changes;
}

void Buffer::_initBetweenReplications() {
	_buffer->clear();
	_buffer->resize(_capacity);
}

unsigned int Buffer::_handlerForSignalDataEvent(SignalData* signalData) {
	// got a signal. Buffer will advance
	trace("Buffer "+this->getName()+" received signal "+signalData->getName());
	Entity* first = _advance(nullptr);
	if (first != nullptr) {
		trace("Buffer entities moved forward");
		_parentModel->sendEntityToComponent(first, this->getConnections()->getFrontConnection());
	}
	return 1;
}


void Buffer::_createInternalAndAttachedData() {
	PluginManager* pm = _parentModel->getParentSimulator()->getPlugins();
	//attached
	if (_advanceOn == AdvanceOn::Signal) {
		if (_attachedSignal  == nullptr) {
			if (_parentModel->isAutomaticallyCreatesModelDataDefinitions()) {
				_attachedSignal = pm->newInstance<SignalData>(_parentModel, getName() + "." + "SignalData");
			}
		}
		SignalData::SignalDataEventHandler handler = SignalData::SetSignalDataEventHandler<Buffer>(&Buffer::_handlerForSignalDataEvent, this);
		_attachedSignal->addSignalDataEventHandler(handler, this);
		_attachedDataInsert("SignalData", _attachedSignal);
	} else {
		_attachedDataRemove("SignalData");
	}
}

void Buffer::_addProperty(PropertyBase* property) {
}


SignalData *Buffer::getsignal() const {
	return _attachedSignal;
}

void Buffer::setSignal(SignalData *newSignal) {
	_attachedSignal = newSignal;
}

unsigned int Buffer::getcapacity() const {
	return _capacity;
}

void Buffer::setCapacity(unsigned int newCapacity) {
	_capacity = newCapacity;
}

Buffer::AdvanceOn Buffer::getadvanceOn() const {
	return _advanceOn;
}

void Buffer::setAdvanceOn(Buffer::AdvanceOn newAdvanceOn) {
	_advanceOn = newAdvanceOn;
}

Buffer::ArrivalOnFullBufferRule Buffer::getarrivalOnFullBufferRule() const {
	return _arrivalOnFullBufferRule;
}

void Buffer::setArrivalOnFullBufferRule(Buffer::ArrivalOnFullBufferRule newArrivalOnFullBufferRule){
	_arrivalOnFullBufferRule = newArrivalOnFullBufferRule;
}

Entity* Buffer::_advance(Entity* enteringEntity) {
	Entity *result = _buffer->front();
	_buffer->erase(_buffer->begin());
	_buffer->push_back(enteringEntity);
	return result;
}
