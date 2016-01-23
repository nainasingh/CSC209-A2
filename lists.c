izeof(no[i]);
374     count += 3;
375     count += sizeof(maybe[i]);
376     count += 2;
377   }
378
379   char *result = (char *)malloc(sizeof(char) * count); // TODO; find require    d size first
380   if (result == NULL){
381     exit(1);
382   }
383   strcpy(result, title);
384
385   for (i = 0; i < n; i++) {
386     char *line = NULL;
387     asprintf(&line, "%s \t Y:%d N:%d M:%d\n", poll->slot_labels[i], yes[i],     no[i], maybe[i]);
388     strcat(result, line);
389   }
390   return result;
391
392 }
