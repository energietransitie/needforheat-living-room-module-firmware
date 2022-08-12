#include <scd41_task.hpp>

#include <scd41.h>
#include <scheduler.hpp>
#include <secure_upload.hpp>
#include <measurements.hpp>

auto secureUploadQueue = SecureUpload::Queue::GetInstance();

void SCD41Task(void *taskInfo)
{
	auto taskName = Scheduler::GetName(taskInfo).c_str();

	// Add formatters for all the measurements.
	Measurements::Measurement::AddFormatter("CO2concentration", "%u");
	Measurements::Measurement::AddFormatter("roomTemp", "%.1f");
	Measurements::Measurement::AddFormatter("relativeHumidity", "%.1f");

	co2_init(SCD41_ADDR);

	// Do three measurements. We do not save the first two.
	uint16_t scd41Data[3] = {};
	for (int i = 0; i < 3; i++)
	{
		co2_read(SCD41_ADDR, scd41Data);
	}

	// Only queue the measurements when the data is not 0.
	// The data is set to 0 when there was a crc error.

	if (scd41Data[0] != 0)
	{
		Measurements::Measurement co2Concentration("CO2concentration", scd41Data[0]);
		secureUploadQueue.AddMeasurement(co2Concentration);
	}
	else
		ESP_LOGW(taskName, "CRC for CO2concentration was incorrect or no sensor wat attached.");

	if (scd41Data[1] != 0)
	{
		Measurements::Measurement co2Concentration("roomTemp", scd41_temp_raw_to_celsius(scd41Data[1]));
		secureUploadQueue.AddMeasurement(co2Concentration);
	}
	else
		ESP_LOGW(taskName, "CRC for roomTemp was incorrect or no sensor wat attached.");

	if (scd41Data[2] != 0)
	{
		Measurements::Measurement co2Concentration("relativeHumidity", scd41_rh_raw_to_percent(scd41Data[2]));
		secureUploadQueue.AddMeasurement(co2Concentration);
	}
	else
		ESP_LOGW(taskName, "CRC for relativeHumidity was incorrect or no sensor wat attached.");
}
