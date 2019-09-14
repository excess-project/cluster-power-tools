!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Copyright (C) 2014-2019 University of Stuttgart
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


program power_calculate
use power_type
use power_timestep
use power_input_parameter
use power_rawdata
use power_statistic
use power_operations
use power_hpc_config
use power_hpc_config_interface
use power_energy_data
use power_profile
implicit none
  type(power_input_parameter_type)                   :: input_parameters !user input
  integer(kind=ik)                                   :: err_code
  type(power_timestep_type),allocatable,dimension(:) :: volt_timesteps
  type(power_timestep_type),allocatable,dimension(:) :: current_timesteps
  type(power_rawdata_type)                           :: current_channel_rawdata
  type(power_rawdata_type)                           :: current_channel_rawdata_filtered
  type(power_rawdata_type)                           :: volt_channel_rawdata
  type(power_rawdata_type)                           :: volt_channel_rawdata_filtered
  logical(kind=lk)                                   :: write_out
  integer(kind=ik)                                   :: error_code
  type(iniPowerConfig_t)                             :: config
  type(components_t),dimension(100)                  :: components
  integer(kind=ik)                                   :: current_board_id
  integer(kind=ik)                                   :: volt_board_id
  real(kind=rk)                                      :: phase_start_sec
  real(kind=rk)                                      :: phase_start_microsec
  real(kind=rk)                                      :: phase_start_nanosec
  real(kind=rk)                                      :: phase_end_sec
  real(kind=rk)                                      :: phase_end_microsec
  real(kind=rk)                                      :: phase_end_nanosec
  real(kind=rk)                                      :: offset_sec
  real(kind=rk)                                      :: offset_microsec
  real(kind=rk)                                      :: offset_nanosec
  integer(kind=ik)                                   :: volt_start_indx
  integer(kind=ik)                                   :: volt_end_indx
  integer(kind=ik)                                   :: current_start_indx
  integer(kind=ik)                                   :: current_end_indx
  real(kind=rk)                                      :: volt_seq_interval_sec
  real(kind=rk)                                      :: current_seq_interval_sec
  real(kind=rk)                                      :: volt_measure_interval_sec
  real(kind=rk)                                      :: current_measure_interval_sec
  real(kind=rk)                                      :: current_first_measurement_sec
  real(kind=rk)                                      :: volt_first_measurement_sec
  real(kind=rk)                                      :: current_last_measurement_sec
  real(kind=rk)                                      :: volt_last_measurement_sec
  type(power_timesteps_sec_type)                     :: volt_timesteps_sec
  type(power_timesteps_sec_type)                     :: current_timesteps_sec
  integer(kind=ik)                                   :: length_tmp
  integer(kind=ik)                                   :: current_num_measures
  integer(kind=ik)                                   :: volt_num_measures
  integer(kind=ik)                                   :: num_phases
  type(power_energy_type)                            :: energy
  real(kind=rk)                                      :: energy_tmp
  real(kind=rk)                                      :: tmp_value, tmp_value2
  real(kind=rk)                                      :: min_power_tmp, max_power_tmp
  real(kind=rk)                                      :: energy_by_average_tmp
  real(kind=rk)                                      :: coeff1_tmp
  real(kind=rk)                                      :: coeff2_tmp
  real(kind=rk)                                      :: current_average_tmp
  real(kind=rk)                                      :: volt_average_tmp
  integer(kind=ik)                                   :: ii,jj,kk
  integer(kind=ik)                                   :: num_components
  type(profile_type)                                 :: profiles
  logical(kind=lk)                                   :: one_channel
  integer(kind=ik)                                   :: win_left, win_right
  real(kind=rk)                                      :: corr_shunt_ohm

  write_out = .false._lk
  one_channel = .false._lk
  energy_tmp = 0.0_rk
  max_power_tmp = 0.0_rk
  min_power_tmp = HUGE(0.0_rk)
  call power_input_parameter_read(input_parameters,err_code)!read user input parameters from command line
  if (input_parameters%verbosity) then
    call power_input_parameter_print(input_parameters)!print user input parameters from command line
  end if
  error_code = hpc_read_power_config_file(input_parameters%config_file, config,components ) 
  num_components = int(config%nPowerComponents,kind=ik)
  
  if(input_parameters%with_profile_file) then
   call power_read_profile_from_file(input_parameters, profiles)
   num_phases = profiles%num_profiles
  else
    num_phases = 1_ik
    phase_start_sec=input_parameters%phase_start_sec
    phase_start_microsec=input_parameters%phase_start_microsec
    phase_start_nanosec=input_parameters%phase_start_nanosec
    phase_end_sec=input_parameters%phase_end_sec
    phase_end_microsec=input_parameters%phase_end_microsec
    phase_end_nanosec=input_parameters%phase_end_nanosec
    offset_sec=input_parameters%offset_sec
    offset_microsec=input_parameters%offset_microsec
    offset_nanosec=input_parameters%offset_nanosec
    
    phase_start_sec = phase_start_sec + phase_start_microsec*1.0e-6 + phase_start_nanosec*1.0e-9
    phase_end_sec = phase_end_sec + phase_end_microsec*1.0e-6 + phase_end_nanosec*1.0e-9
    offset_sec = offset_sec + offset_microsec*1.0e-6 + offset_nanosec*1.0e-9
  end if
  write(power_output_unit,'(A,I0)') "num_phases:", num_phases
  if(phase_start_sec .gt. phase_end_sec) then
   write(power_output_unit,'(A,E13.6,A,E13.6)')  "Error: The phase_start_time is later than phase_end_time: ",&
   phase_start_sec, " > ", phase_end_sec
         
   stop
  end if
      
  if(input_parameters%with_profile_file) then
   num_phases = profiles%num_profiles
  else
    num_phases = 1_ik
    call power_profile_new(1_ik,profiles)
    profiles%id(1)=1_ik
    profiles%start_sec(1)=input_parameters%phase_start_sec
    profiles%start_microsec(1)=input_parameters%phase_start_microsec
    profiles%start_nanosec(1)=input_parameters%phase_start_nanosec
    profiles%end_sec(1)=input_parameters%phase_end_sec
    profiles%end_microsec(1)=input_parameters%phase_end_microsec
    profiles%end_nanosec(1)=input_parameters%phase_end_nanosec
    profiles%offset_sec=input_parameters%offset_sec
    profiles%offset_microsec=input_parameters%offset_microsec
    profiles%offset_nanosec=input_parameters%offset_nanosec
  end if
  offset_sec=profiles%offset_sec
  offset_microsec=profiles%offset_microsec
  offset_nanosec=profiles%offset_nanosec
    
  call power_energy_new(energy,num_phases,num_components)
  
  current_board_id = -1_ik
  volt_board_id = -1_ik
  do ii=1_ik,num_components
    if(input_parameters%verbosity) then
         write(power_output_unit,'(A,A)') "Check power data for the componetent:",&
           c_to_f_string(components(ii)%label)
    end if
    if(components(ii)%voltage_board_id .eq. components(ii)%current_board_id .and. &
        components(ii)%voltage_channel_id .eq. components(ii)%current_channel_id ) then
      one_channel = .true._lk
    else
      one_channel = .false._lk
    endif
      
    if(current_board_id .ne. components(ii)%current_board_id) then
       current_board_id = components(ii)%current_board_id
       if(input_parameters%verbosity) then
         write(power_output_unit,'(A,I0)') "Read current timesteps for board:",current_board_id
       end if
       input_parameters%board_id=components(ii)%current_board_id
       call power_timesteps_read(current_timesteps,input_parameters,err_code)
       if(err_code .ne. 0_ik) then
         write(power_output_unit,'(A,I0)') &
          "Error power_timesteps_read executed with error:",err_code
         stop
        end if
        call power_timesteps_to_timesteps_sec(current_timesteps,offset_sec,input_parameters,&
          current_timesteps_sec,current_seq_interval_sec,err_code)
        if(err_code .ne. 0_ik) then
         write(power_output_unit,'(A,I0)') &
          "Error power_timesteps_read executed with error:",err_code
         stop
        end if
     end if
     if(volt_board_id .ne. components(ii)%voltage_board_id) then
       volt_board_id = components(ii)%voltage_board_id
       if(input_parameters%verbosity) then
         write(power_output_unit,'(A,I0)') "Read voltage timesteps for board:",current_board_id
       end if
       call power_timesteps_read(volt_timesteps,input_parameters,err_code)
       if(err_code .ne. 0_ik) then
         write(power_output_unit,'(A,I0)') &
          "Error power_timesteps_read executed with error:",err_code
         stop
        end if
        call power_timesteps_to_timesteps_sec(volt_timesteps,offset_sec,input_parameters,&
          volt_timesteps_sec,volt_seq_interval_sec,err_code)
        if(err_code .ne. 0_ik) then
         write(power_output_unit,'(A,I0)') &
          "Error power_timesteps_read executed with error:",err_code
         stop
        end if
     end if

    if(input_parameters%verbosity) then
      write(power_output_unit,'(A,A)') "Read power current data for the componetent:",&
           c_to_f_string(components(ii)%label)
    end if
    input_parameters%channel_id=components(ii)%current_channel_id
    input_parameters%board_id=components(ii)%current_board_id
    call power_power_rawdata_read(current_channel_rawdata,current_timesteps,input_parameters,err_code)
    if(err_code .ne. 0_ik) then
       write(power_output_unit,'(A,I0)') &
         "Error power_power_rawdata_read(current) executed with error:",err_code
       stop
    end if
    win_left=input_parameters%window_step_left
    win_right=input_parameters%window_step_right
    input_parameters%window_step_left=input_parameters%window_step_left_a
    input_parameters%window_step_right=input_parameters%window_step_right_a
    call power_statistic_rawdata_filter_median(current_channel_rawdata_filtered,current_channel_rawdata,input_parameters,err_code);
    if(err_code .ne. 0_ik) then
       write(power_output_unit,'(A,I0)') &
         "Error power_statistic_rawdata_filter_median(current) executed with error:",err_code
       stop
    end if
    input_parameters%window_step_left=win_left
    input_parameters%window_step_right=win_right
    current_channel_rawdata%rawdata=current_channel_rawdata_filtered%rawdata
    if(input_parameters%verbosity) then
      write(power_output_unit,'(A,A)') "Read power voltage data for the componetent:",&
           c_to_f_string(components(ii)%label)
    end if
    input_parameters%channel_id = components(ii)%voltage_channel_id
    input_parameters%board_id = components(ii)%voltage_board_id
    if(input_parameters%channel_id .lt. 0) then
      if(allocated(volt_channel_rawdata%rawdata)) deallocate(volt_channel_rawdata%rawdata)
      length_tmp = size(current_channel_rawdata%rawdata)
      allocate(volt_channel_rawdata%rawdata(length_tmp))
      volt_channel_rawdata%rawdata_seq_num=current_channel_rawdata%rawdata_seq_num
      volt_channel_rawdata%rawdata=12.0_rk
    else
      call power_power_rawdata_read(volt_channel_rawdata,volt_timesteps,input_parameters,err_code)
      if(err_code .ne. 0_ik) then
         write(power_output_unit,'(A,I0)') &
          "Error power_power_rawdata_read(voltage) executed with error:",err_code
         stop
       end if
      call power_statistic_rawdata_filter_mean(volt_channel_rawdata_filtered,volt_channel_rawdata,input_parameters,err_code);
      if(err_code .ne. 0_ik) then
       write(power_output_unit,'(A,I0)') &
         "Error power_statistic_rawdata_filter_mean(voltage) executed with error:",err_code
       stop
      end if
      volt_channel_rawdata%rawdata = volt_channel_rawdata_filtered%rawdata
    end if

    !find indx_start and indx_end
    current_measure_interval_sec = current_seq_interval_sec / real(current_channel_rawdata%rawdata_seq_num,kind=rk)
    volt_measure_interval_sec = volt_seq_interval_sec / real(volt_channel_rawdata%rawdata_seq_num,kind=rk)
    if(input_parameters%verbosity) then
      write(power_output_unit,'(2(A,E13.6))') "current_measure_interval_sec:",&
          current_measure_interval_sec,&
          "; volt_measure_interval_sec:",  volt_measure_interval_sec
    end if
    current_first_measurement_sec = current_timesteps_sec%sec(1_ik) - &
      current_measure_interval_sec*current_channel_rawdata%rawdata_seq_num
    volt_first_measurement_sec = volt_timesteps_sec%sec(1_ik) - &
      volt_measure_interval_sec*volt_channel_rawdata%rawdata_seq_num
    length_tmp = size(current_timesteps_sec%sec)
    current_last_measurement_sec = current_timesteps_sec%sec(length_tmp)
    length_tmp = size(volt_timesteps_sec%sec)
    volt_last_measurement_sec = volt_timesteps_sec%sec(length_tmp)
    current_num_measures = size(current_channel_rawdata%rawdata)
    volt_num_measures = size(volt_channel_rawdata%rawdata)
    
    if (input_parameters%apply_shunt_correction) then
      corr_shunt_ohm = components(ii)%shunt_ohm
    else
      corr_shunt_ohm = 0.0_rk
    endif
    coeff1_tmp = components(ii)%coeff1
    coeff2_tmp = components(ii)%coeff2
    do jj=1_ik,num_phases
    
      phase_start_sec=profiles%start_sec(jj)
      phase_start_microsec=profiles%start_microsec(jj)
      phase_start_nanosec=profiles%start_nanosec(jj)
      phase_end_sec=profiles%end_sec(jj)
      phase_end_microsec=profiles%end_microsec(jj)
      phase_end_nanosec=profiles%end_nanosec(jj)
      
      phase_start_sec = phase_start_sec + phase_start_microsec*1.0e-6 + phase_start_nanosec*1.0e-9
      phase_end_sec = phase_end_sec + phase_end_microsec*1.0e-6 + phase_end_nanosec*1.0e-9
      offset_sec = offset_sec + offset_microsec*1.0e-6 + offset_nanosec*1.0e-9    
    
      if(phase_start_sec .gt. phase_end_sec) then
        write(power_output_unit,'(A,E13.6,A,E13.6)')  &
        "Error: The phase_start_time is later than phase_end_time: ",&
        phase_start_sec, " > ", phase_end_sec
             
        stop
      end if
      volt_start_indx = 0
      volt_end_indx = 0
      current_start_indx = 0
      current_end_indx = 0
      
      if(input_parameters%relative_time) then
          current_start_indx = int(&
            (phase_start_sec+&
            current_measure_interval_sec)/current_measure_interval_sec,kind=ik)
          volt_start_indx = int(&
            (phase_start_sec+&
            volt_measure_interval_sec)/volt_measure_interval_sec,kind=ik)
          current_end_indx = int(&
            (phase_end_sec+&
            current_measure_interval_sec)/current_measure_interval_sec,kind=ik)
          volt_end_indx = int(&
            (phase_end_sec+&
            volt_measure_interval_sec)/volt_measure_interval_sec,kind=ik)
      else
          current_start_indx = int(&
            (phase_start_sec-current_first_measurement_sec+&
            current_measure_interval_sec)/current_measure_interval_sec,kind=ik)
          volt_start_indx = int(&
            (phase_start_sec-volt_first_measurement_sec+&
            volt_measure_interval_sec)/volt_measure_interval_sec,kind=ik)
          current_end_indx = int(&
            (phase_end_sec-current_first_measurement_sec+&
            current_measure_interval_sec)/current_measure_interval_sec,kind=ik)
          volt_end_indx = int(&
            (phase_end_sec-volt_first_measurement_sec+&
            volt_measure_interval_sec)/volt_measure_interval_sec,kind=ik)
      end if
      if(input_parameters%verbosity) then
        write(power_output_unit,'(2(A,E26.10))')"phase_start_sec:",&
            phase_start_sec,"; phase_end_sec",phase_end_sec 
        write(power_output_unit,'(3(A,E26.10))')"phase_start_sec(int):",&
            profiles%start_sec(jj),&
            "; profiles%start_microsec(jj)",profiles%start_microsec(jj),&    
            "; profiles%start_nanosec(jj)",profiles%start_nanosec(jj)
        write(power_output_unit,'(2(A,I0,A,I0))')"current_indx:",&
            current_start_indx,"-",current_end_indx ,&
             "; volt_indx:",&
            volt_start_indx,"-",volt_end_indx
      end if  
      !calculate power consumption from voltage and current data during the jj-th phase
      if(volt_start_indx .eq. current_start_indx .and. &
        current_end_indx .eq. volt_end_indx) then
        energy_tmp=0.0
        if(input_parameters%verbosity) then
          write(power_output_unit,'(2(A,I0,A,I0))')"current_indx:",&
            current_start_indx,"-",current_end_indx ,&
             "; volt_indx:",&
            volt_start_indx,"-",volt_end_indx
        end if
        max_power_tmp = 0.0_rk
        min_power_tmp = HUGE(0.0_rk)
        if(one_channel)then
          do kk=volt_start_indx, volt_end_indx
            tmp_value2 = current_channel_rawdata%rawdata(kk)
            tmp_value = coeff1_tmp*tmp_value2 - tmp_value2*tmp_value2*corr_shunt_ohm
            if(max_power_tmp .lt. tmp_value) then
              max_power_tmp = tmp_value
            end if
            if(min_power_tmp .gt. tmp_value) then
              min_power_tmp = tmp_value
            end if
            energy_tmp = energy_tmp + &
            (current_channel_rawdata%rawdata(kk))
          end do
          energy_tmp = energy_tmp / real(((volt_end_indx-volt_start_indx)+1_ik),kind=rk)
          energy_tmp= energy_tmp+coeff2_tmp
        else
          do kk=volt_start_indx, volt_end_indx
            tmp_value2 = current_channel_rawdata%rawdata(kk)
            tmp_value = tmp_value2*volt_channel_rawdata%rawdata(kk)
            tmp_value = tmp_value - tmp_value2*tmp_value2*corr_shunt_ohm
            if(max_power_tmp .lt. tmp_value) then
              max_power_tmp = tmp_value
            end if
            if(min_power_tmp .gt. tmp_value) then
              min_power_tmp = tmp_value
            end if
            energy_tmp = energy_tmp + tmp_value
          end do
          energy_tmp = energy_tmp / real(((volt_end_indx-volt_start_indx)+1_ik),kind=rk)
          energy_tmp= energy_tmp*coeff1_tmp+coeff2_tmp
        end if
      end if
      current_average_tmp = 0.0_rk
      volt_average_tmp = 0.0_rk
      do kk=current_start_indx, current_end_indx
        current_average_tmp = current_average_tmp + &
          (current_channel_rawdata%rawdata(kk))
      end do
      current_average_tmp = current_average_tmp / real(((current_end_indx-current_start_indx)+1_ik),kind=rk)
      do kk=volt_start_indx, volt_end_indx
        volt_average_tmp = volt_average_tmp + &
          volt_channel_rawdata%rawdata(kk)
      end do
      volt_average_tmp = volt_average_tmp / real(((volt_end_indx-volt_start_indx)+1_ik),kind=rk)
      if(one_channel)then
        energy_by_average_tmp= coeff1_tmp*(current_average_tmp)+coeff2_tmp-&
                &current_average_tmp*current_average_tmp*corr_shunt_ohm
      else
        energy_by_average_tmp= coeff1_tmp*(volt_average_tmp*current_average_tmp)+coeff2_tmp-&
                &current_average_tmp*current_average_tmp*corr_shunt_ohm
      end if
      !if(jj .eq. 1_ik) then
        write(energy%energy_by_phases(jj)%phase_label,'(A,I0)')trim("phase:"),profiles%id(jj)
        energy%energy_by_phases(jj)%phase_start=phase_start_sec
        energy%energy_by_phases(jj)%phase_end=phase_end_sec
        energy%energy_by_phases(jj)%phase_duration=phase_end_sec-phase_start_sec
        energy%energy_by_phases(jj)%phase_num_measures=(volt_end_indx-volt_start_indx)+1_ik
        energy%energy_by_phases(jj)%relative_time=input_parameters%relative_time          
        energy%energy_by_phases(jj)%volt_start_indx=volt_start_indx
        energy%energy_by_phases(jj)%volt_end_indx=volt_end_indx
        energy%energy_by_phases(jj)%current_start_indx=current_start_indx
        energy%energy_by_phases(jj)%current_end_indx=current_end_indx


      !end if
      
      energy%energy_by_phases(jj)%energy_by_components(ii)%component_label=&
        c_to_f_string(components(ii)%label)
      energy%energy_by_phases(jj)%energy_by_components(ii)%power_watt=energy_tmp
      energy%energy_by_phases(jj)%energy_by_components(ii)%max_power_watt=max_power_tmp
      energy%energy_by_phases(jj)%energy_by_components(ii)%min_power_watt=min_power_tmp
      energy%energy_by_phases(jj)%energy_by_components(ii)%power_by_average_watt=energy_by_average_tmp
      energy%energy_by_phases(jj)%energy_by_components(ii)%energy_joule=&
        energy_tmp*(phase_end_sec-phase_start_sec)
      energy%energy_by_phases(jj)%energy_by_components(ii)%max_energy_joule=&
        max_power_tmp*(phase_end_sec-phase_start_sec)
      energy%energy_by_phases(jj)%energy_by_components(ii)%min_energy_joule=&
        min_power_tmp*(phase_end_sec-phase_start_sec)
      energy%energy_by_phases(jj)%energy_by_components(ii)%energy_by_average_joule=&
        energy_by_average_tmp*(phase_end_sec-phase_start_sec)
  end do
    
  end do
  
  call power_energy_print(energy)  
  call power_energy_write(energy,input_parameters)
  call power_energy_idx_write(energy,input_parameters)
  
end program power_calculate
