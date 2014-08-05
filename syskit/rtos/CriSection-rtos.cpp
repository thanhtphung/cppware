/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <FreeRTOS.h>
#include <task.h>
#include "syskit/CriSection.hpp"
#include "syskit/macros.h"

BEGIN_NAMESPACE1(syskit)


CriSection::CriSection()
{
    cs_ = 0;
}


CriSection::CriSection(unsigned int /*spinCount*/)
{
}


CriSection::~CriSection()
{
}


//!
//! Lock this critical section. Do not wait.
//! Return true if successful.
//!
bool CriSection::tryLock()
{
    bool ok = false; //TODO
    return ok;
}


//!
//! Lock this critical section. Wait forever if necessary.
//!
void CriSection::lock()
{
    taskENTER_CRITICAL();
}


//!
//! Unlock this critical section.
//!
void CriSection::unlock()
{
    taskEXIT_CRITICAL();
}

END_NAMESPACE1
