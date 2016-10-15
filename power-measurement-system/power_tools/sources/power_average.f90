!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Copyright (C) 2014-2015 University of Stuttgart
!
! Licensed under the Apache License, Version 2.0 (the "License");
! you may not use this file except in compliance with the License.
! You may obtain a copy of the License at
!
!     http://www.apache.org/licenses/LICENSE-2.0
!
! Unless required by applicable law or agreed to in writing, software
! distributed under the License is distributed on an "AS IS" BASIS,
! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
! See the License for the specific language governing permissions and
! limitations under the License.
!
!Bugs to: khabi@hlrs.de
!High Performance Computing Center Stuttgart (HLRS)
!University of Stuttgart
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



program power_average
use power_type
use power_timestep
use power_input_parameter
use power_rawdata
use power_statistic
use power_operations
implicit none
  type(power_input_parameter_type)                   :: input_parameters !user input
  integer(kind=ik)                                   :: err_code
  type(power_timestep_type),allocatable,dimension(:) :: timesteps
  type(power_rawdata_type)                           :: channel_rawdata
  type(power_rawdata_type)                           :: channel_av_rawdata
  real(kind=rk)                                      :: average
  logical(kind=lk)                                   :: write_out
  integer(kind=ik)                                   :: num_records

  
  write_out = .false._lk
  call power_input_parameter_read(input_parameters,err_code)!read user input parameters from command line
  if (input_parameters%verbosity) then
    call power_input_parameter_print(input_parameters)!read user input parameters from command line
  end if
  
    
  call power_timesteps_read(timesteps,input_parameters,err_code)
  if (input_parameters%verbosity) then
    call power_timesteps_print(timesteps,err_code)
  endif
  num_records = size(timesteps)
  
  call power_power_rawdata_read(channel_rawdata,timesteps,input_parameters,err_code)
  call power_power_rawdata_average(average,channel_rawdata,err_code)
  if (input_parameters%verbosity) then
    write(power_output_unit,'(A,E16.6)'  ) "Average of raw data:", average
  endif
  select case(trim(input_parameters%operation))
  case ("median")
    if (input_parameters%verbosity) then
      write(power_output_unit,'(A,E16.6)'  ) "Apply median filter"
    end if
    call power_statistic_rawdata_filter_median(channel_av_rawdata,channel_rawdata,input_parameters,err_code)
  case ("mean")
    if (input_parameters%verbosity) then
      write(power_output_unit,'(A,E16.6)'  ) "Apply mean filter"
    end if
    call power_statistic_rawdata_filter_mean(channel_av_rawdata,channel_rawdata,input_parameters,err_code)
  case ("average")
    if (input_parameters%verbosity) then
      write(power_output_unit,'(A,E16.6)'  ) "Apply average filter"
    endif
    call power_statistic_rawdata_filter_average(channel_av_rawdata,channel_rawdata,input_parameters,err_code)
  case ("no")
    if (input_parameters%verbosity) then
      write(power_output_unit,'(A,E16.6)'  ) "No filter applied"
    endif
    call power_statistic_rawdata_filter_eq(channel_av_rawdata,channel_rawdata,input_parameters,err_code)
  case default
    if (input_parameters%verbosity) then
      write(power_output_unit,'(A,A)'  ) & 
          "Apply average, median, mean or no filter and concatenate power-raw-data - ", trim(input_parameters%operation)
    endif
    err_code = 1_ik
  end select
  if(err_code .eq. 0_ik) write_out = .true._lk
  call power_power_rawdata_average(average,channel_av_rawdata,err_code)
  if (input_parameters%verbosity) then
    write(power_output_unit,'(A,E16.6)'  ) "Average of filtered data:", average
  endif
  if(write_out) then
    call power_power_rawdata_write(channel_av_rawdata,num_records,input_parameters%out_prefixchannel,&
      input_parameters,err_code)
  end if
  !rawdata_length = size(channel_rawdata%rawdata)
  !norm2_original = NORM2(channel_rawdata%rawdata(1:rawdata_length))
  !norm2_filter = NORM2(channel_av_rawdata%rawdata(1:rawdata_length))

  !call power_operation_find_min_alpha(alpha, norm_2, rest_rawdata,channel_rawdata,channel_av_rawdata,input_parameters,err_code)
  !write(power_output_unit,'(2(A,E16.6))'  ) "original norm2",norm2_original,"; filtered norm2:", norm2_filter
  !write(power_output_unit,'(2(A,E16.6))'  ) "min alpha",alpha,"; norm2:", norm_2
  !call power_power_rawdata_write(rest_rawdata,num_records,input_parameters%out_diff_prefixchannel,&
  !    input_parameters,err_code)
  ! call power_statistic_rawdata_filter_mean(channel_av_rawdata,rest_rawdata,input_parameters,err_code)
  !if(write_out) then
  !  call power_power_rawdata_write(channel_av_rawdata,num_records,"diff_filter",&
  !    input_parameters,err_code)
  !  call power_power_rawdata_write(channel_rawdata,num_records,"original",&
  !    input_parameters,err_code)
  !end if
end program
