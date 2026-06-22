#include "PlayerCameraHook.h"
#include "menu.h"

namespace Hooks
{
    void AutoVanityStateHook::Install()
    {

        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_AutoVanityState[0] };
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        logger::info("Hook installed");

    }

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState)

    {
        /*float rotBefore = a_this->autoVanityRot;*/
        _Update(a_this, a_nextState);
        logger::info("Hook fired");
        a_this->autoVanityRot = 0.0f;
        logger::info("Camera rotation set to 0");
    }
}