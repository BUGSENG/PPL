-verbose
-project_name=getenv("ECLAIR_PROJECT_NAME")
-project_root=getenv("ECLAIR_PROJECT_ROOT")

-setq=data_dir,getenv("ECLAIR_DATA_DIR")

-enable=B.REPORT.ECB
-config=B.REPORT.ECB,output=join_paths(data_dir,"FRAME.@FRAME@.ecb")
-config=B.REPORT.ECB,preprocessed=show
-config=B.REPORT.ECB,macros=10

-enable=B.EXPLAIN

-enable=NC3.1.2.a
-enable=NC3.1.3.a
-enable=NC3.1.3.b
-enable=NC3.3.1.a
-enable=NC3.3.1.b
-enable=NC3.3.1.c
-enable=NC3.3.1.d
-enable=NC3.3.1.e
-enable=NC3.3.1.f
-enable=NC3.3.1.g
-enable=NC3.3.1.h
-enable=NC3.3.1.i
-enable=NC3.3.1.j
-enable=NC3.3.1.k
-enable=NC3.3.1.l
-enable=NC3.3.1.m
-enable=NC3.3.2.a
-enable=NC3.3.2.b
-enable=NC3.3.2.c
-enable=NC3.3.2.d
-enable=NC3.3.3.a
-enable=NC3.3.3.b
-enable=NC3.3.3.c
-enable=NC3.3.4.a
-enable=NC3.3.4.b
-enable=NC3.3.5.a
-enable=NC3.3.6.a
-enable=NC3.3.6.b
-enable=NC3.8.3.a

-eval_file=common_config.ecl

-reports={hide,all_exp_external}
