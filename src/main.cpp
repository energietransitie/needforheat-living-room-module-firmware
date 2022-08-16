#include <generic_esp_32.hpp>
#include <generic_tasks.hpp>
#include <scheduler.hpp>
#include <scd41_task.hpp>
#include <presence_detection.hpp>

constexpr const char *DEVICE_TYPE_NAME = "CO2-meter-SCD4x";

extern "C" void app_main(void)
{
	// Add the generic tasks to the scheduler,
	// heartbeat, timesync and optionally presence detection and OTA updates.
	GenericTasks::AddTasksToScheduler();

#ifdef M5STACK_COREINK
	// This is a fake MAC-address.
	PresenceDetection::AddMacAddress("ab:ff:0c:fc:ba:5c");

	// Add the SCD41 task to the scheduler.
	Scheduler::AddTask(SCD41Task, "SCD41 task", 4096, nullptr, 1, Scheduler::Interval::MINUTES_10);
#endif // M5STACK_COREINK

	// Initialize generic firmware.
	GenericESP32Firmware::Initialize(DEVICE_TYPE_NAME);

	// Start the scheduler.
	Scheduler::Start();
}
