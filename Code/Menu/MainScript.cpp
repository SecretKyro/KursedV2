#include "MainScript.hpp"
#include "../GTA/Script/ScriptFiber.hpp"
#include "../GTA/Script/ScriptManager.hpp"
#include "Lists.hpp"
#include "../GTA/Invoker/Natives.hpp"
#include "../Utility/Timer.hpp"
#include "../Utility/CustomText.hpp"
#include "../UI/UIManager.hpp"
#include "../UI/BoolOption.hpp"
#include "../UI/ChooseOption.hpp"
#include "../UI/NumberOption.hpp"
#include "../UI/RegularOption.hpp"
#include "../UI/SubOption.hpp"
#include "../UI/RegularSubmenu.hpp"
#include "../UI/PlayerSubmenu.hpp"
#include "../Features/Local/Local.hpp"

namespace Base
{
	enum Submenu : std::uint32_t
	{
		Home,
		Self,
		Movement,
		Test,
		Network,
		PlayerList,
		Settings,
		SettingsBar,
		SettingsOption,
		SettingsFooter,
		SettingsInput,
		SelectedPlayer,
		Debug
	};

	void MainScript::Initialize()
	{
		using namespace UserInterface;
		Invoker::CacheHandlers();

		g_UiManager->AddSubmenu<RegularSubmenu>("Home", Home, [](RegularSubmenu* sub)
			{
				sub->AddOption<SubOption>("Self", nullptr, Self);
				sub->AddOption<SubOption>("Weapon", nullptr, Test);
				sub->AddOption<SubOption>("Network", nullptr, Network);
				sub->AddOption<SubOption>("Spawner", nullptr, Test);
				sub->AddOption<SubOption>("Miscellaneous", nullptr, Test);
				sub->AddOption<SubOption>("Settings", nullptr, Settings);
				sub->AddOption<SubOption>("Debug", nullptr, Debug);
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Local", Self, [](RegularSubmenu* sub)
			{
				sub->AddOption<SubOption>("Movement", nullptr, Movement);
				sub->AddOption<BoolOption<bool>>("God Mode", nullptr, &g_LocalFeatures->m_GodMode);
				sub->AddOption<BoolOption<bool>>("Never Wanted", nullptr, &g_LocalFeatures->m_NeverWanted);
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Movement", Movement, [](RegularSubmenu* sub)
			{
				sub->AddOption<SubOption>("Slide Run", nullptr, rage::joaat("SlideRun"));
				sub->AddOption<BoolOption<bool>>("Super Man", nullptr, &g_LocalFeatures->m_SuperMan);
				sub->AddOption<BoolOption<bool>>("Super Jump", nullptr, &g_LocalFeatures->m_SuperJump);
				sub->AddOption<BoolOption<bool>>("Beast Jump", nullptr, &g_LocalFeatures->m_BeastJump);
				sub->AddOption<BoolOption<bool>>("Ninja Jump", nullptr, &g_LocalFeatures->m_NinjaJump);
				sub->AddOption<BoolOption<bool>>("Graceful Landing", nullptr, &g_LocalFeatures->m_GracefulLanding);
				sub->AddOption<BoolOption<bool>>("Fast Run", nullptr, &g_LocalFeatures->m_FastRun);
				sub->AddOption<BoolOption<bool>>("Fast Swim", nullptr, &g_LocalFeatures->m_FastSwim);
				
			});
		g_UiManager->AddSubmenu<RegularSubmenu>("Slide Run", rage::joaat("SlideRun"), [](RegularSubmenu* sub)
			{
				sub->AddOption<BoolOption<bool>>("Enabled", nullptr, &g_LocalFeatures->m_EnableSlideRun);
				sub->AddOption<NumberOption<float>>("Speed", nullptr, &g_LocalFeatures->m_SlideRun, 0.f, 100, 0.1f, 1);
			});
		g_UiManager->AddSubmenu<RegularSubmenu>("Network", Network, [](RegularSubmenu* sub)
			{
				sub->AddOption<SubOption>("Playerlist", nullptr, PlayerList);
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Players", PlayerList, [](RegularSubmenu* sub)
			{
				if (*g_GameVariables->m_IsSessionStarted)
				{
					const auto& NetPlayerMgr = *g_GameVariables->m_NetworkPlayerMgr;

					if (NetPlayerMgr == nullptr)
						return;

					for (const auto& Player : NetPlayerMgr->m_player_list)
					{
						if (Player != nullptr)
						{
							if (Player->is_valid())
							{
								const auto ID = Player->m_player_id;
								sub->AddOption<SubOption>(g_GameFunctions->m_GetPlayerName(ID), nullptr, SelectedPlayer, [=]
									{
										g_SelectedPlayer = ID;
									});
							}
						}
					}
				}
				else
				{
					sub->AddOption<SubOption>(g_GameFunctions->m_GetPlayerName(0), nullptr, SelectedPlayer, [=]
						{
							g_SelectedPlayer = 0;
						});
				}
			});

		g_UiManager->AddSubmenu<PlayerSubmenu>(&g_SelectedPlayer, SelectedPlayer, [](PlayerSubmenu* sub)
			{
				sub;
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Demo", Test, [](RegularSubmenu* sub)
			{
				sub->AddOption<RegularOption>("Option", nullptr, []
					{
						LOG(Info, "You pressed the test option");
					});

				static bool testBool1{};
				sub->AddOption<BoolOption<bool>>("Bool", nullptr, &testBool1);

				static std::int32_t int32Test{ 69 };
				sub->AddOption<NumberOption<std::int32_t>>("Int32", nullptr, &int32Test, 0, 100);

				static std::int64_t int64Test{ 420 };
				sub->AddOption<NumberOption<std::int64_t>>("Int64", nullptr, &int64Test, 0, 1000, 10);

				static float floatTest{ 6.9f };
				sub->AddOption<NumberOption<float>>("Float", nullptr, &floatTest, 0.f, 10.f, 0.1f, 1);

				static std::vector<std::uint64_t> vector{ 1, 2, 3 };
				static std::size_t vectorPos{};

				sub->AddOption<ChooseOption<const char*, std::size_t>>("Array", nullptr, &Lists::DemoList, &Lists::DemoListPos);
				sub->AddOption<ChooseOption<std::uint64_t, std::size_t>>("Vector", nullptr, &vector, &vectorPos);
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Debug", Debug, [](RegularSubmenu* sub)
			{
				sub->AddOption<NumberOption<float>>("Footer Sprite Addition", nullptr, &g_UiManager->m_Test, 0.f, 1000.f, 1.f, 1);
				sub->AddOption<RegularOption>("Answer", nullptr, [] {  LOG(Fatal, "Answer is {} now fuck off static code is bad :mad:", 220.f - g_UiManager->m_Test); });
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Settings", Settings, [](RegularSubmenu* sub)
			{
				sub->AddOption<SubOption>("Infobar", nullptr, SettingsBar);
				sub->AddOption<SubOption>("Options", nullptr, SettingsOption);
				sub->AddOption<SubOption>("Footer", nullptr, SettingsFooter);
				sub->AddOption<SubOption>("Input", nullptr, SettingsInput);
				sub->AddOption<NumberOption<float>>("X Position", nullptr, &g_UiManager->m_PosX, 0.f, 2000.f, 25.f, 2);
				sub->AddOption<NumberOption<float>>("Y Position", nullptr, &g_UiManager->m_PosY, 0.f, 2000.f, 25.f, 2);
				sub->AddOption<NumberOption<float>>("Width", nullptr, &g_UiManager->m_Width, 0.f, 1000.f, 50.f, 2);
				sub->AddOption<BoolOption<bool>>("Sounds", nullptr, &g_UiManager->m_Sounds);
				sub->AddOption<RegularOption>("Unload", nullptr, [] { g_Running = false; });
				sub->AddOption<RegularOption>("Exit", nullptr, [] { exit(0); });
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Infobar", SettingsBar, [](RegularSubmenu* sub)
			{
				sub->AddOption<NumberOption<std::uint8_t>>("Background R", nullptr, &g_UiManager->m_SubmenuBarBackgroundColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background G", nullptr, &g_UiManager->m_SubmenuBarBackgroundColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background B", nullptr, &g_UiManager->m_SubmenuBarBackgroundColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background A", nullptr, &g_UiManager->m_SubmenuBarBackgroundColor.a, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Text R", nullptr, &g_UiManager->m_SubmenuBarTextColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Text G", nullptr, &g_UiManager->m_SubmenuBarTextColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Text B", nullptr, &g_UiManager->m_SubmenuBarTextColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Text A", nullptr, &g_UiManager->m_SubmenuBarTextColor.a, '\0', static_cast<std::uint8_t>(255));
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Options", SettingsOption, [](RegularSubmenu* sub)
			{
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Background R", nullptr, &g_UiManager->m_OptionSelectedBackgroundColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Background G", nullptr, &g_UiManager->m_OptionSelectedBackgroundColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Background B", nullptr, &g_UiManager->m_OptionSelectedBackgroundColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Background A", nullptr, &g_UiManager->m_OptionSelectedBackgroundColor.a, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Background R", nullptr, &g_UiManager->m_OptionUnselectedBackgroundColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Background G", nullptr, &g_UiManager->m_OptionUnselectedBackgroundColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Background B", nullptr, &g_UiManager->m_OptionUnselectedBackgroundColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Background A", nullptr, &g_UiManager->m_OptionUnselectedBackgroundColor.a, '\0', static_cast<std::uint8_t>(255));

				sub->AddOption<NumberOption<std::uint8_t>>("Selected Text R", nullptr, &g_UiManager->m_OptionSelectedTextColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Text G", nullptr, &g_UiManager->m_OptionSelectedTextColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Text B", nullptr, &g_UiManager->m_OptionSelectedTextColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Selected Text A", nullptr, &g_UiManager->m_OptionSelectedTextColor.a, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Text R", nullptr, &g_UiManager->m_OptionUnselectedTextColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Text G", nullptr, &g_UiManager->m_OptionUnselectedTextColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Text B", nullptr, &g_UiManager->m_OptionUnselectedTextColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Unselected Text A", nullptr, &g_UiManager->m_OptionUnselectedTextColor.a, '\0', static_cast<std::uint8_t>(255));
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Footer", SettingsFooter, [](RegularSubmenu* sub)
			{
				sub->AddOption<NumberOption<std::uint8_t>>("Background R", nullptr, &g_UiManager->m_FooterBackgroundColor.r, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background G", nullptr, &g_UiManager->m_FooterBackgroundColor.g, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background B", nullptr, &g_UiManager->m_FooterBackgroundColor.b, '\0', static_cast<std::uint8_t>(255));
				sub->AddOption<NumberOption<std::uint8_t>>("Background A", nullptr, &g_UiManager->m_FooterBackgroundColor.a, '\0', static_cast<std::uint8_t>(255));
			});

		g_UiManager->AddSubmenu<RegularSubmenu>("Input", SettingsInput, [](RegularSubmenu* sub)
			{
				sub->AddOption<NumberOption<std::int32_t>>("Open Delay", nullptr, &g_UiManager->m_OpenDelay, 10, 1000, 10, 0);
				sub->AddOption<NumberOption<std::int32_t>>("Back Delay", nullptr, &g_UiManager->m_BackDelay, 10, 1000, 10, 0);
				sub->AddOption<NumberOption<std::int32_t>>("Enter Delay", nullptr, &g_UiManager->m_EnterDelay, 10, 1000, 10, 0);
				sub->AddOption<NumberOption<std::int32_t>>("Vertical Delay", nullptr, &g_UiManager->m_VerticalDelay, 10, 1000, 10, 0);
				sub->AddOption<NumberOption<std::int32_t>>("Horizontal Delay", nullptr, &g_UiManager->m_HorizontalDelay, 10, 1000, 10, 0);
			});
	}
	void MainScript::Tick()
	{
		g_MainScript->Initialize();
		while (true)
		{
			g_UiManager->GameTick();
			Script::Current()->Yield();
		}
	}
}
