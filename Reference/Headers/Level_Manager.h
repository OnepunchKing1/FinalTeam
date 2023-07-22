#pragma once
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
    DECLARE_SINGLETON(CLevel_Manager)
private:
    CLevel_Manager();
    virtual ~CLevel_Manager() = default;

public:
    HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);   //레벨 이동 시 사용할 것
    void    Tick_Level(_double dTimeDelta);

private:
    class CLevel*   m_pCurrentLevel = { nullptr };  //현재 레벨
    _uint           m_iLevelIndex = { 0 };

public:
    virtual void Free() override;
};

END