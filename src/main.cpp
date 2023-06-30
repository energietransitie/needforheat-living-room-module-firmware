#include <generic_esp_32.hpp>
#include <generic_tasks.hpp>
#include <scheduler.hpp>
#include <scd41_task.hpp>
#include <presence_detection.hpp>
#include <scd41.h>
#include <util/buttons.hpp>
#include <util/buzzer.hpp>

constexpr const char *DEVICE_TYPE_NAME = "twomes-co2-occupancy-scd41-m5coreink-firmware";

void CalibrateCO2()
{
	ESP_LOGD("Main", "Calibrating CO2 sensor.");

	// Buzz the buzzer for 200 ms to signal we are calibrating.
	Buzzer::Buzz(200);

	co2_init(SCD41_ADDR);

	int16_t offset;
	auto err = co2_force_recalibration(SCD41_ADDR, &offset);
	if (err != ESP_OK)
	{
		ESP_LOGD("Main", "Calibration failed.");

		// Buzz the buzzer for 1000 ms, 3 times to signal calibration unsuccessful.
		Buzzer::Buzz(1000, 3);

		return;
	}

	ESP_LOGD("Main", "Calibration done and set to 415 ppm. Offset was %d", offset);

	// Buzz the buzzer for 200 ms, 2 times to signal calibration successful.
	Buzzer::Buzz(200, 2);
}

extern "C" void app_main(void)
{
	// Configure GPIO for calibration button.
	gpio_config_t gpioConfigButton = {};
	gpioConfigButton.intr_type = GPIO_INTR_NEGEDGE;
	gpioConfigButton.mode = GPIO_MODE_INPUT;
	gpioConfigButton.pin_bit_mask = GPIO_SEL_37;
	gpioConfigButton.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpioConfigButton.pull_up_en = GPIO_PULLUP_ENABLE;
	auto err = gpio_config(&gpioConfigButton);
	Error::CheckAppendName(err, "Main", "An error occured when configuring GPIO for calibration button.");

	Buttons::ButtonPressHandler::AddButton(GPIO_NUM_37, "Calibrate C02", 0, nullptr, CalibrateCO2);

	// Configure the buzzer.
	Buzzer::Configure();

	// Add the SCD41 task to the scheduler.
	Scheduler::AddTask(SCD41Task, "SCD41 task", 4096, nullptr, 1, Scheduler::Interval::MINUTES_10);

	// Add the generic tasks to the scheduler,
	// heartbeat, timesync and optionally presence detection and OTA updates.
	GenericTasks::AddTasksToScheduler();

	// Initialize generic firmware.
	GenericESP32Firmware::Initialize(DEVICE_TYPE_NAME);

	// Start the scheduler.
	Scheduler::Start();
}
