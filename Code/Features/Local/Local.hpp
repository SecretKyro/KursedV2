#pragma once 
#include "../../Common.hpp"

namespace Base
{
	class LocalFeatures
	{
	public:
		void Tick()
		{
			SuperMan(m_SuperMan);
			NinjaJump(m_NinjaJump);
			FastRun(m_FastRun);
			FastSwim(m_FastSwim);
			NeverWanted(m_NeverWanted);
			GodMode(m_GodMode);
			SlideRun(m_EnableSlideRun, m_SlideRun);
		}

		bool m_SuperMan = false;
		bool m_SuperJump = false; // TaskJumpConstructor Hook
		bool m_BeastJump = false; // TaskJumpConstructor Hook
		bool m_NinjaJump = false;
		bool m_GracefulLanding = false; // FallTaskConstructor Hook
		bool m_FastRun = false;
		bool m_FastSwim = false;
		bool m_GodMode = false;
		bool m_NeverWanted = false;
		bool m_EnableSlideRun = false;
		float m_SlideRun = 1.f;
	private:
		void SuperMan(bool enable);
		void NinjaJump(bool enable);
		void FastRun(bool enable);
		void FastSwim(bool enable);
		void GodMode(bool enable);
		void NeverWanted(bool enable);
		void SlideRun(bool enable, float value);
	};

	inline std::unique_ptr<LocalFeatures> g_LocalFeatures;
}