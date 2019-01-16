package com.springboot.repositroy;

import com.springboot.entities.Wendu;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * Created by 默默 on 2017/5/8.
 */
@Repository
public interface WenduRepositoty extends JpaRepository<Wendu,Integer>{
    @Query("select wendu,time from Wendu w order by w.time desc ")
    List<Wendu> findAllWendu();
    @Query("select shidu,time from Wendu w order by w.time desc ")
    List<Wendu> findAllShidu();
    //void addWendu();
}
