                        inning[i].overs[cnt-1].ndel++;
                }
                if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
                        i = 1, cnt = 0;
                }
                lncnt++;
        }
        printf("inning 1 start: %d\n", inning[0].start);
        //______________________________________________________________________________________________________________________
                for(i=0; i<2; i++){
                        for(cnt=0; cnt < inning[i].novers; cnt++){
                                fwrite((buf+line[inning[i].overs[cnt].bowler_name.cnl].start_index+inning[i].overs[cnt].bowler_name.start), 1, line[inning[i].overs[cnt].bowl
                        }
                }
        //      c=0, i=0, cnt=0;
        //      unsigned char cnt0=1;
        //      lncnt = inning[0].start; //reusing the lncnt
        //      while(lncnt < nl){
        //              if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-2]) == ':')){
        //                      cnt++, cnt0 = 1;
        //              }
        //              if(((buf[line[lncnt].start_index+line[lncnt].length-4]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-2]) == ':')){
        //                      inning[i].overs[cnt-1].ds[cnt0].del_no.cnl = lncnt, inning[i].overs[cnt-1].ds[cnt0].del_no.start = 6;
        //              }
        //              if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
        //                      i = 1, cnt = 0;
        //              }
        //              lncnt++;
        //      }
        //      printf("inning[0].overs[1].ds[2].cnl: %d\n", inning[0].overs[0].ds[2].del_no.cnl);
        //      printf("inning[1].overs[1].ds[2].cnl: %d\n", inning[1].overs[0].ds[2].del_no.cnl);
        free(buf);
        free(line);

        return 0;
}

                                                                                                                                                           218,0-1       Bot

