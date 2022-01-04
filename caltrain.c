#include "pintos_thread.h"
#include <stdio.h>
struct station
{
	int ghe_trong;
	int khach_doi_tau;
	int khach_len_tau;
	struct lock *khoa;
	struct condition *tat_ca_khach_len_tau;
	struct condition *tau_den;
};
void station_init(struct station *sta)
{
	sta->ghe_trong = 0;
	sta->khach_doi_tau = 0;
	sta->khach_len_tau = 0;
	sta->khoa = malloc(sizeof(struct lock));
	lock_init(sta->khoa);
	sta->tat_ca_khach_len_tau = malloc(sizeof(struct condition));
	cond_init(sta->tat_ca_khach_len_tau);
	sta->tau_den = malloc(sizeof(struct condition));
	cond_init(sta->tau_den);
};
void station_load_train(struct station *station, int count)
{
	lock_acquire(station->khoa);
	station->ghe_trong = count;

	while ((station->khach_doi_tau > 0) && (station->ghe_trong > 0))
	{
		cond_broadcast(station->tau_den, station->khoa);
		cond_wait(station->tat_ca_khach_len_tau, station->khoa);
	}

	station->ghe_trong = 0;
	lock_release(station->khoa);
}

void station_wait_for_train(struct station *station)
{
	lock_acquire(station->khoa);
	station->khach_doi_tau++;
	while (station->khach_len_tau == station->ghe_trong || station->ghe_trong == 0)
	{
		cond_wait(station->tau_den, station->khoa);
	}
	station->khach_doi_tau--;
	station->khach_len_tau++;
	lock_release(station->khoa);
}
void station_on_board(struct station *station)
{
	lock_acquire(station->khoa);
	station->khach_len_tau--;
	station->ghe_trong--;
	if ((station->ghe_trong == 0) || (station->khach_len_tau == 0))
	{
		cond_signal(station->tat_ca_khach_len_tau, station->khoa);
	}

	lock_release(station->khoa);
}
