#include "philo.h"

static void	mutex_destroy(t_table *table, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < table->n_philos) // フォークのミューテックスを破棄
	{
		pthread_mutex_destroy(&table->forks[i]);
		pthread_mutex_destroy(&philos[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&table->died); // 誰かが死んだかどうかを管理するミューテックスを破棄
	pthread_mutex_destroy(&table->moments_nourished_lock);
}

static int	make_threads(t_philo *philos, t_table *table)
{
	int	i;

	table->start_time = get_time(); // 最後に食べた時間を現在の時間に設定
	i = 0;
	while (i < table->n_philos)
	{
		philos[i].table = table; // 各フィロにテーブルの情報を渡す
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
			return (1); // スレッドの作成に失敗した場合
		i++;
	}
	usleep(100); // スレッドが開始するまで少し待つ
	if (pthread_create(&table->monitor_thread, NULL, monitor, philos) != 0)
		return (3);
	i = 0;
	while (i < table->n_philos)
	{
		if (pthread_join(philos[i].thread, NULL) != 0) // 各スレッドの終了を待つ
			return (2);
		i++;
	}
	pthread_join(table->monitor_thread, NULL); // 監視スレッドの終了を待つ
	mutex_destroy(table, philos);              // ミューテックスの破棄を行う
	return (0);                                // 成功
}

int	main(int argc, char **argv) // numPhilo, die, eat, sleep,
{
	t_table table;
	t_philo philos[PHILO_MAX];
	int res;

	if (argc < 5 || argc > 6) // 引数の数が5か6以外の場合はエラー
		return (1);
	if (valid_or_not(argc, argv))
		return (1);
	//入力が数字のみか、INTMAX超えてないか確認の関数➺リターンがNUMBERなら、atoiで数値に➺200？
	arg_init(argv, &table);
	table.start_time = get_time();    // 開始時間を現在の時間に設定
	philo_init(philos, argv, &table); // Philoの数だけID,table,lastmealを初期化する関数
	// ストラクチャに代入する関数↑
	mutex_init(&table);
	// mutexを初期化する関数
	res = make_threads(philos, &table); // threadsを作成する関数 intreturnどうすんの
	return (res);                       // 成功なら0,失敗なら1,2,3を返す
}
