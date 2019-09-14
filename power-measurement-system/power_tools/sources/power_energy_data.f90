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


module power_energy_data

use power_type
use power_timestep
use power_input_parameter
use power_hpc_config
use power_rawdata
implicit none

type power_energy_component_type
 character(len=str_length)               :: component_label
 real(kind=rk)                           :: power_watt
 real(kind=rk)                           :: min_power_watt
 real(kind=rk)                           :: max_power_watt
 real(kind=rk)                           :: std_power_watt
 real(kind=rk)                           :: power_by_average_watt
 real(kind=rk)                           :: energy_joule
 real(kind=rk)                           :: min_energy_joule
 real(kind=rk)                           :: max_energy_joule
 real(kind=rk)                           :: std_energy_joule
 real(kind=rk)                           :: energy_by_average_joule

end type power_energy_component_type

type power_energy_phase_type
 character(len=str_length)                                       :: phase_label
 real(kind=rk)                                                   :: phase_start
 real(kind=rk)                                                   :: phase_end
 real(kind=rk)                                                   :: phase_duration
 integer(kind=ik)                                                :: phase_num_measures
 type(power_energy_component_type),allocatable, dimension(:)     :: energy_by_components
 logical(kind=lk)                                                :: relative_time
 integer(kind=ik)                                                :: volt_start_indx
 integer(kind=ik)                                                :: volt_end_indx
 integer(kind=ik)                                                :: current_start_indx
 integer(kind=ik)                                                :: current_end_indx

end type power_energy_phase_type

type power_energy_type
 integer(kind=ik)                                            :: num_phases
 type(power_energy_phase_type),allocatable, dimension(:)     :: energy_by_phases
end type power_energy_type
 
 
interface new
  module procedure power_energy_new
end interface new

interface del
  module procedure power_energy_del
end interface del

contains

subroutine power_energy_new(power_energy, num_phases, num_components)
 type(power_energy_type),intent(out) :: power_energy
 integer(kind=ik),intent(in)         :: num_phases
 integer(kind=ik),intent(in)         :: num_components
 integer(kind=ik) :: ii, jj
 

 allocate(power_energy%energy_by_phases(num_phases))
 do ii=1,num_phases
   allocate(power_energy%energy_by_phases(ii)%energy_by_components(num_components))
   do jj=1,num_components
      power_energy%energy_by_phases(ii)%energy_by_components(jj)%component_label="undef"
      power_energy%energy_by_phases(ii)%energy_by_components(jj)%energy_joule=0.0_rk
      power_energy%energy_by_phases(ii)%energy_by_components(jj)%energy_by_average_joule=0.0_rk
      power_energy%energy_by_phases(ii)%energy_by_components(jj)%power_watt=0.0_rk
      power_energy%energy_by_phases(ii)%energy_by_components(jj)%power_by_average_watt=0.0_rk
   end do
   power_energy%energy_by_phases(ii)%phase_start=0.0_rk
   power_energy%energy_by_phases(ii)%phase_end=0.0_rk
   power_energy%energy_by_phases(ii)%phase_duration=0.0_rk
   power_energy%energy_by_phases(ii)%phase_num_measures=0_ik
   power_energy%energy_by_phases(ii)%relative_time=.false._lk
 end do
end subroutine power_energy_new

subroutine power_energy_del(power_energy)
 type(power_energy_type),intent(out) :: power_energy
 integer(kind=ik) :: ii, num_phases
 
 if(allocated(power_energy%energy_by_phases)) then
   num_phases = size(power_energy%energy_by_phases)
   do ii=1,num_phases
     if(allocated(power_energy%energy_by_phases(ii)%energy_by_components)) then
       deallocate(power_energy%energy_by_phases(ii)%energy_by_components)
      end if
   end do
 end if
end subroutine power_energy_del

subroutine power_energy_print(power_energy)
  type(power_energy_type),intent(in) :: power_energy
  integer(kind=ik) :: ii, jj, num_phases, num_components
  
  num_phases = size(power_energy%energy_by_phases)
  do ii=1,num_phases
    num_components = size(power_energy%energy_by_phases(ii)%energy_by_components)
    write(power_output_unit,'(A,A,A)') "=====",&
      trim(power_energy%energy_by_phases(ii)%phase_label),"====="
    write(power_output_unit,'(A,L)') "relative_time:",&
      power_energy%energy_by_phases(ii)%relative_time
    write(power_output_unit,'(A,E13.6,A)') "phase_start:",&
      (power_energy%energy_by_phases(ii)%phase_start), " sec"
    write(power_output_unit,'(A,E13.6,A)') "phase_end:",&
      (power_energy%energy_by_phases(ii)%phase_end), " sec"
    write(power_output_unit,'(A,E13.6,A)') "phase_duration:",&
      (power_energy%energy_by_phases(ii)%phase_duration), " sec"
    write(power_output_unit,'(A,E13.6)') "phase_num_measures:",&
      real(power_energy%energy_by_phases(ii)%phase_num_measures,kind=rk)
    do jj=1,num_components
      write(power_output_unit,'(A,A,A)') char(9)//"==component:",&
       trim(power_energy%energy_by_phases(ii)%energy_by_components(jj)%component_label),&
       "=="
      write(power_output_unit,'(A,E13.6,A)') char(9)//"average power:",&
       (power_energy%energy_by_phases(ii)%energy_by_components(jj)%power_watt), " watt"
      write(power_output_unit,'(A,E13.6,A)') char(9)//"power_by_average_joule:",&
       (power_energy%energy_by_phases(ii)%energy_by_components(jj)%power_by_average_watt), " watt"
      write(power_output_unit,'(A,E13.6,A)') char(9)//"energy:",&
       (power_energy%energy_by_phases(ii)%energy_by_components(jj)%energy_joule), " J"
      write(power_output_unit,'(A,E13.6,A)') char(9)//"energy_by_average_joule:",&
       (power_energy%energy_by_phases(ii)%energy_by_components(jj)%energy_by_average_joule), " J"
      write(power_output_unit,'(A,A,A)') char(9)//"==",&
       "==",&
       "=="
    end do
    write(power_output_unit,'(A,A,A)') "=====",&
      "=====","====="
  end do

end subroutine power_energy_print

subroutine power_energy_write(power_energy,input_parameters)
  type(power_energy_type),intent(in)            :: power_energy
  type(power_input_parameter_type),intent(in)   :: input_parameters !user input
  integer(kind=ik) :: ii, jj, num_phases, num_components
  character(len=str_length) :: filename
  logical(kind=lk) :: show_header
  integer :: unit,iostat
  
  write(filename,'(A,A,A)') trim(input_parameters%out_topdir),trim('/'),&
   trim(input_parameters%out_filename)
  unit = 3233
  show_header = .false._lk
  open(unit=unit,iostat=iostat,file=filename,status='NEW',action='READWRITE')
  write(power_output_unit,'(A,A,A,I0)')'check the output file:"',&
        trim(filename),'" exists (>0-old file exists): ', iostat
  if(iostat .eq. 0) then
    show_header = .true._lk
  else
    open(unit=unit,iostat=iostat,file=filename,status='OLD',action='READWRITE',position='append')
    write(power_output_unit,'(A,A,A,I0)')'reopen the profile file:"',&
        trim(filename),'" status (0-ok): ', iostat
    show_header = .false._lk
  end if
  num_components = size(power_energy%energy_by_phases(1_ik)%energy_by_components)
  if (iostat .gt. 0) then
    write(power_output_unit,'(A)')'power_energy_write: error, could not open the file:'
    write(power_output_unit,'(A)') trim(filename)
  else
    if(show_header) then 
      write(unit,'(A)',advance='no') "ID;"
      write(unit,'(A)',advance='no') "Time;"
      write(unit,'(A)',advance='no') "num_measure;"
      do jj=1,num_components
        write(unit,'(A,A,A)',advance='no') &
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Watt") ,trim(";")
        write(unit,'(A,A,A,A)',advance='no') trim("min_"),& 
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Watt") ,trim(";")
        write(unit,'(A,A,A,A)',advance='no') trim("max_"),&
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Watt") ,trim(";")
        write(unit,'(A,A,A)',advance='no') &
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Joule"),trim(";")
        write(unit,'(A,A,A,A)',advance='no') trim("min_"),&
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Joule"),trim(";")
        write(unit,'(A,A,A,A)',advance='no') trim("max_"),&
         trim(power_energy%energy_by_phases(1_ik)%energy_by_components(jj)%component_label),&
         trim("_Joule"),trim(";")
      end do
      write(unit,'(A)',advance='yes') " "
    end if
    num_phases = size(power_energy%energy_by_phases)
    do ii=1_ik,num_phases
      num_components = size(power_energy%energy_by_phases(ii)%energy_by_components)
      write(unit,'(A,A)',advance='no') trim(power_energy%energy_by_phases(ii)%phase_label),";"
      write(unit,'(E13.6,A)',advance='no') power_energy%energy_by_phases(ii)%phase_duration,";"
      write(unit,'(E13.6,A)',advance='no') &
        real(power_energy%energy_by_phases(ii)%phase_num_measures,kind=rk),";"
      do jj=1,num_components
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%power_watt,";"
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%min_power_watt,";"
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%max_power_watt,";"
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%energy_joule,";"
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%min_energy_joule,";"
        write(unit,'(E13.6,A)',advance='no') &
          power_energy%energy_by_phases(ii)%energy_by_components(jj)%max_energy_joule,";"
      end do
      write(unit,'(A)',advance='yes') " "
    end do
    close(unit)
  end if

end subroutine power_energy_write

subroutine power_energy_idx_write(power_energy,input_parameters)
  type(power_energy_type),intent(in)            :: power_energy
  type(power_input_parameter_type),intent(in)   :: input_parameters !user input
  integer(kind=ik) :: ii, num_phases, num_components
  character(len=str_length) :: filename
  logical(kind=lk) :: show_header
  integer :: unit,iostat
  
  write(filename,'(A,A,A)') trim(input_parameters%out_topdir),trim('/'),&
   trim(input_parameters%out_idx_filename)
  unit = 3233
  show_header = .false._lk
  open(unit=unit,iostat=iostat,file=filename,status='NEW',action='READWRITE')
  write(power_output_unit,'(A,A,A,I0)')'check the output file:"',&
        trim(filename),'" exists (>0-old file exists): ', iostat
  if(iostat .eq. 0) then
    show_header = .true._lk
  else
    open(unit=unit,iostat=iostat,file=filename,status='OLD',action='READWRITE',position='append')
    write(power_output_unit,'(A,A,A,I0)')'reopen the profile file:"',&
        trim(filename),'" status (0-ok): ', iostat
    show_header = .false._lk
  end if
  num_components = size(power_energy%energy_by_phases(1_ik)%energy_by_components)
  if (iostat .gt. 0) then
    write(power_output_unit,'(A)')'power_energy_write: error, could not open the file:'
    write(power_output_unit,'(A)') trim(filename)
  else
    if(show_header) then 
      write(unit,'(A)',advance='no') "ID;"
      write(unit,'(A)',advance='no') "Time;"
      write(unit,'(A)',advance='no') "num_measure;"
      write(unit,'(A,A,A)',advance='no') trim("start_volt_"),trim("idx"),trim(";")
      write(unit,'(A,A,A)',advance='no') trim("end_volt_"),trim("idx"),trim(";")
      write(unit,'(A,A,A)',advance='no') trim("start_current_"),trim("idx"),trim(";")
      write(unit,'(A,A,A)',advance='no') trim("end_current_"),trim("idx"),trim(";")
      write(unit,'(A)',advance='yes') " "
    end if
    num_phases = size(power_energy%energy_by_phases)
    do ii=1_ik,num_phases
      num_components = size(power_energy%energy_by_phases(ii)%energy_by_components)
      write(unit,'(A,A)',advance='no') trim(power_energy%energy_by_phases(ii)%phase_label),";"
      write(unit,'(E13.6,A)',advance='no') power_energy%energy_by_phases(ii)%phase_duration,";"
      write(unit,'(I10,A)',advance='no') &
          power_energy%energy_by_phases(ii)%phase_num_measures,";"
      write(unit,'(I10,A)',advance='no') &
          power_energy%energy_by_phases(ii)%volt_start_indx,";"
      write(unit,'(I10,A)',advance='no') &
          power_energy%energy_by_phases(ii)%volt_end_indx,";"
      write(unit,'(I10,A)',advance='no') &
          power_energy%energy_by_phases(ii)%current_start_indx,";"
      write(unit,'(I10,A)',advance='no') &
          power_energy%energy_by_phases(ii)%current_end_indx,";"
      write(unit,'(A)',advance='yes') " "
    end do
    close(unit)
  end if

end subroutine power_energy_idx_write



end module power_energy_data
