#power of nodeXX
args <- commandArgs(TRUE)
data_filename<-toString(args[1])
pdf_directory<-toString(args[2])
JOBID<-toString(args[3])
NODE<-"nodeXX"

print("data_filename")
print(data_filename)
print("pdf_directory")
print(pdf_directory)
print("JOBID")
print(JOBID)
print("NODE")
print(NODE)

library(Hmisc)
library(stringr)
library(magicaxis)
library(data.table)

power_colors=c("red","darkgreen","black","darkturquoise","blue","gray34", "magenta3")
power_colors_text=c("red","green","black","darkturquoise","blue","gray34", "magenta3")
power_labels=c("CPU1", "CPU2", "SDRAM-CPU1", "SDRAM-CPU2")
power_pchs=c(3,4,8,9,13,11,10,12)

DATA <- read.table(data_filename, sep=";",header = TRUE )
num_rows<-nrow(DATA)

if(identical(NODE,"node03"))
{
 #legend_str<-paste("E5-2680v3; TeslaK80 (GPU+ATX12V-9.0 W);")
 legend_str<-paste("2 x E5-2680v3 and 8 x 16 GB SDRAM4;")
}else if(identical(NODE,"node01"))
{
 legend_str<-paste("E5-2690v2; TeslaK40 (GPU+ATX12V-4.8 W)")
}else if(identical(NODE,"node02"))
{
 legend_str<-paste("E5-2690v2; AMD FirePro S10000")
}

loop_start<-1
loop_end<-1
power_pdf_filename_tmp<-eval(expression(paste(pdf_directory,"/","power_all_",JOBID,".pdf")))
power_pdf_filename<-str_replace_all(power_pdf_filename_tmp,fixed(" "),"")
perf_pdf_filename_tmp<-eval(expression(paste(pdf_directory,"/","perf_all_",JOBID,".pdf")))
perf_pdf_filename<-str_replace_all(perf_pdf_filename_tmp,fixed(" "),"")
energy_pdf_filename_tmp<-eval(expression(paste(pdf_directory,"/","energy_all_",JOBID,".pdf")))
energy_pdf_filename<-str_replace_all(energy_pdf_filename_tmp,fixed(" "),"")

pdf(power_pdf_filename)
power_x<-seq(from = 1, to = num_rows, by = 1)
power_y<-cbind(DATA$CPU1_nodeXX_Watt-4.0*DATA$SDRAM_CPU1_nodeXX_Watt,DATA$CPU2_nodeXX_Watt-4.0*DATA$SDRAM_CPU2_nodeXX_Watt,4.0*DATA$SDRAM_CPU1_nodeXX_Watt,4.0*DATA$SDRAM_CPU2_nodeXX_Watt)
power_type<-cbind("p","p","p","p","p")
xlim_min=1.0
xlim_max=num_rows
ylim_min=0.0
ylim_max=170.0
matplot(power_x,power_y,col = power_colors ,pch=power_pchs,type=power_type,ylab = "",xlab = "", xlim=c(xlim_min,xlim_max),ylim=c(ylim_min,ylim_max), cex=0.5,cex.lab=0.5, cex.axis=0.5, cex.main=0.5,  cex.sub=0.5,yaxt="n",xaxt="n")
#par(new = TRUE)
title_str=paste("Electric Power of ", NODE, ";","\n","job_id:",JOBID)
title(main=title_str, xlab="phase id", ylab="Power (W)",cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5) 
axis(1,labels=NULL,cex.axis=1.5,cex=1.5)
axis(2, at = c(0,20,40,60,80,100,120,140,160),cex.axis=1.5,cex=1.5)
grid(nx = NULL, ny = NULL, col = "gray8", lty = "dotted", lwd = par("lwd"), equilogs = TRUE)
legend(xlim_min,ylim_max,title=legend_str,legend=power_labels,inset=.05, fill=power_colors,ncol=3, box.col="gray48",lwd=2,cex=1.1, lty=0)
minor.tick(nx=10, ny=10, tick.ratio=0.1)

pdf(perf_pdf_filename)
perf_colors=c("red")
perf_colors_text=c("blue")
perf_pchs=c(3)
perf_type=c("p")
perf_x<-seq(from = 1, to = num_rows, by = 1)
print("DATA$flops")
print(DATA$flops)
print("DATA$num_tests")
print(DATA$num_tests)
print("DATA$num_threads")
print(DATA$num_threads)
print("DATA$time_sec")
print(DATA$time_sec)
perf_y<-cbind((((DATA$flops*DATA$num_tests)/DATA$time_sec)/1.0e9)*DATA$num_threads)
xlim_min=1.0
xlim_max=num_rows
ylim_min=0.0
ylim_max=max(perf_y)*1.3
matplot(perf_x,perf_y,col = perf_colors ,pch=perf_pchs,type=perf_type,ylab = "",xlab = "", xlim=c(xlim_min,xlim_max),ylim=c(ylim_min,ylim_max), cex=0.5,cex.lab=1.0, cex.axis=1.0, cex.main=1.0,  cex.sub=1.0,yaxt="n",xaxt="n")
title_str=paste("Performance of ", NODE, ";","\n","job_id:",JOBID)
title(main=title_str, xlab="phase id", ylab="Performance (GFlops)",cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5)
axis(1,labels=NULL,cex.axis=1.5,cex=1.5)
axis(2,labels=NULL,cex.axis=1.5,cex=1.5)
grid(nx = NULL, ny = NULL, col = "gray8", lty = "dotted", lwd = par("lwd"), equilogs = TRUE)
minor.tick(nx=10, ny=10, tick.ratio=0.1)

pdf(energy_pdf_filename)
energy_x<-seq(from = 1, to = num_rows, by = 1)
perf_dbl=as.double(DATA$flops*DATA$num_tests)
perf_dbl=perf_dbl*as.double(DATA$num_threads)
perf_dbl=perf_dbl/as.double(DATA$time_sec)
energy_y<-cbind((DATA$CPU1_nodeXX_Watt/(perf_dbl))*1.0e9  ,(DATA$CPU2_nodeXX_Watt/(perf_dbl))*1.0e9, (DATA$SDRAM_CPU1_nodeXX_Watt/(perf_dbl))*1.0e9, (DATA$SDRAM_CPU2_nodeXX_Watt/(perf_dbl))*1.0e9)
xlim_min=1.0
xlim_max=num_rows
ylim_min=0.0
ylim_max=max(energy_y)*1.3
matplot(energy_x,energy_y,col = power_colors ,pch=power_pchs,type=power_type,ylab = "",xlab = "", xlim=c(xlim_min,xlim_max),ylim=c(ylim_min,ylim_max), cex=0.5,cex.lab=1.0, cex.axis=1.0, cex.main=1.0,  cex.sub=1.0,yaxt="n",xaxt="n")
title_str=paste("Energy of ", NODE, ";","\n","job_id:",JOBID)
title(main=title_str, xlab="phase id", ylab="Joule/GFlop",cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5)
axis(1,labels=NULL,cex.axis=1.5,cex=1.5)
axis(2,at=c(10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180),cex.axis=1.5,cex=1.5)
legend(xlim_min,ylim_max,title=legend_str,legend=power_labels,inset=.05, fill=power_colors,ncol=3, box.col="gray48",lwd=2,cex=1.1, lty=0)
grid(nx = NULL, ny = NULL, col = "gray8", lty = "dotted", lwd = par("lwd"), equilogs = TRUE)
minor.tick(nx=10, ny=10, tick.ratio=0.1)
