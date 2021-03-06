/*
 * The MIT License
 *
 * Copyright 2017-2018 Norwegian University of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING  FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <fmi4cpp/fmi2/import/fmi2ModelExchangeFmu.hpp>
#include <fmi4cpp/fmi2/import/fmi2ModelExchangeInstance.hpp>

using namespace std;

using namespace fmi4cpp;
using namespace fmi4cpp::fmi2;
using namespace fmi4cpp::solver;

fmi2ModelExchangeFmu::fmi2ModelExchangeFmu(const shared_ptr<FmuResource> &resource,
                                   const shared_ptr<ModelExchangeModelDescription> &md)
        : resource_(resource), modelDescription_(md) {}


shared_ptr<ModelExchangeModelDescription> fmi2ModelExchangeFmu::getModelDescription() const {
    return modelDescription_;
}

std::unique_ptr<fmi2ModelExchangeInstance> fmi2ModelExchangeFmu::newInstance(bool visible, bool loggingOn) {
    shared_ptr<fmi2ModelExchangeLibrary> lib = nullptr;
    string modelIdentifier = modelDescription_->modelIdentifier();
    if (modelDescription_->canBeInstantiatedOnlyOncePerProcess()) {
        lib = make_shared<fmi2ModelExchangeLibrary>(modelIdentifier, resource_);
    } else {
        if (lib_ == nullptr) {
            lib_ = make_shared<fmi2ModelExchangeLibrary>(modelIdentifier, resource_);
        }
        lib = lib_;
    }
    fmi2Component c = lib->instantiate(modelIdentifier, fmi2ModelExchange, guid(),
                                       resource_->getResourcePath(), visible, loggingOn);
    return make_unique<fmi2ModelExchangeInstance>(c, lib, modelDescription_);
}

std::unique_ptr<fmi2ModelExchangeSlave>
fmi2ModelExchangeFmu::newInstance(std::unique_ptr<ModelExchangeSolver> &solver, bool visible, bool loggingOn) {
    unique_ptr<fmi2ModelExchangeInstance> instance = newInstance(visible, loggingOn);
    return make_unique<fmi2ModelExchangeSlave>(instance, solver);
}
