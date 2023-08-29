﻿<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);
$mode = "입력";
$action = "manager_insert.php";

if (array_key_exists("MID", $_GET)) {
    $MID = $_GET["MID"];
    $query =  "select * from Manager where Manager.MID = $MID";
    $result = mysqli_query($conn, $query);
    $Manager = mysqli_fetch_array($result);
    if(!$Manager) {
        msg("관리자가 존재하지 않습니다.!!");
    }
    $mode = "수정";
    $action = "manager_modify.php";
}

?>
    <div class="container">
        <form name="manager_form" action="<?=$action?>" method="post" class="fullwidth">
            <input type="hidden" name="MID" value="<?=$Manager['MID']?>"/>
            <h3>관리자 정보 <?=$mode?></h3>

            <p>
                <label for="manager_id">관리자 아이디</label>
                <input type="text" placeholder="관리자 아이디 입력" id="manager_id" name="manager_id" value="<?=$Manager['아이디']?>"/>
            </p>
            <p>
                <label for="manager_password">관리자 비밀번호</label>
                <input type="password" placeholder="관리자 비밀번호 입력" id="manager_password" name="manager_password" value="<?=$Manager['비밀번호']?>"/>
            </p>
            <p>
                <label for="manager_nickname">닉네임</label>
                <input placeholder="관리자 닉네임 입력" id="manager_nickname" name="manager_nickname"  value="<?=$Manager['닉네임']?>" />
            </p>
            <p>
                <label for="manager_name">이름</label>
                <input placeholder="관리자 이름 입력" id="manager_name" name="manager_name"  value="<?=$Manager['이름']?>" />
            </p>        
            <p>
                <label for="manager_phone">전화번호</label>
                <input placeholder="관리자 전화번호 입력" id="manager_phone" name="manager_phone"  value="<?=$Manager['전화번호']?>" />
            </p>
            
            
            <p align="center"><button class="button primary large" onclick="javascript:return validate();"><?=$mode?></button></p>

           <script>
    		function validate() {
                    var phonePattern = /^010\d{8}$/;
    				var phoneInput = document.getElementById("manager_phone").value;
                    if(document.getElementById("manager_id").value == "") {
                        alert ("아이디를 입력해 주십시오"); return false;
                    }
                    else if(document.getElementById("manager_password").value == "") {
                        alert ("비밀번호를 입력해 주십시오"); return false;
                    }
                    else if(document.getElementById("manager_nickname").value == "") {
                        alert ("닉네임을 입력해 주십시오"); return false;
                    }
                    else if(document.getElementById("manager_name").value == "") {
                        alert ("이름을 입력해 주십시오"); return false;
                    }
                    else if( phoneInput == "") {
                        alert ("전화번호를 입력해 주십시오"); return false;
                    }else if (!phonePattern.test(phoneInput)) {
        				alert("전화번호는 010xxxxxxxx 형식으로 입력해 주십시오");
        				return false;
    				}
                    return true;
        
    	        }
         </script>

        </form>
    </div>
<? include("footer.php") ?>